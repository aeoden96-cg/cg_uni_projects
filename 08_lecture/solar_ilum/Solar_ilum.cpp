// Kompajliranje:
// g++ -o Solar_ilum Solar_ilum.cpp util_t.cpp -lGLEW -lGL -lGLU -lglut -lpthread

#ifdef _WIN32
#include <windows.h>             //bit ce ukljuceno ako se koriste windows
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stack> 

#include <GL/glew.h>
#include <GL/freeglut.h>

// Ukljuci potporu za osnovne tipove glm-a: vektore i matrice
#include <glm/glm.hpp>

// Ukljuci funkcije za projekcije, transformacije pogleda i slicno
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// Ako ste na Ubuntu-u i program se odmah po pokretanju srusi sa segmentation fault,
// radi se o poznatom bugu:
// https://bugs.launchpad.net/ubuntu/+source/nvidia-graphics-drivers-319/+bug/1248642
// potreban je sljedeci define i program treba linkati s -lpthread:
#ifndef _WIN32
#define LINUX_UBUNTU_SEGFAULT
#endif

#ifdef LINUX_UBUNTU_SEGFAULT
//ss1
#include <pthread.h>
#endif

// Nasa pomocna biblioteka za ucitavanje, prevodenje i linkanje programa shadera
#include "util_t.hpp"
#include "Solar_ilum.hpp"

//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window; 

GLuint vertexArrayID;
GLuint programID;
GLuint MVMatrixID,PMatrixID,NMatrixID;
GLuint AmbientalID,EyeDirectionID,IsLocalID,TwoSidedID; 
GLuint MatIndexID; 

GLuint mylightbuffer; 

glm::mat4 projection;

static GLenum spinMode = GL_TRUE;
static GLenum singleStep = GL_FALSE;
const int fps = 60;

// These three variables control the animation's state and speed.
static float HourOfDay = 0.0;
static float DayOfYear = 0.0;
static float AnimateIncrement = 2.0;  // Time step for animation (hours)

int sjencanje = 1, mode = 1;

struct Clock
{
	void setWaitTime( float seconds ){ end = clock() + seconds*CLOCKS_PER_SEC;	}
	void wait( void ){ for( ; clock() < end; ) ;	}
	float end;
} stoperica;

// Osvjetljenje

struct LightProperties {
	glm::vec3 ambient;
	GLfloat spotCosCutoff;
	glm::vec3 diffColor;
	GLfloat spotExponent;
	glm::vec3 specColor;
	GLfloat constantAttenuation;
	glm::vec3 position;
	GLfloat linearAttenuation;
	glm::vec3 halfVector;
	GLfloat quadraticAttenuation;
	glm::vec3 coneDirection;
	GLboolean isEnabled;
	GLboolean paddingl1[3];
	GLboolean isPositional;
	GLboolean paddingl2[3];
	GLboolean isSpot;
	GLboolean paddingl3[3];
	GLfloat paddingl4[2];
};

struct MaterialProperties {
	glm::vec4 materialColor;
	glm::vec3 emission;
	GLfloat shininess;
	glm::vec3 ambient;
	GLfloat paddingm1;
	glm::vec3 diffuse;
	GLfloat paddingm2;
	glm::vec3 specular;
	GLfloat paddingm3;
};

LightProperties lights[3];
MaterialProperties material[5];
glm::vec3 ambiental = glm::vec3(0.4f, 0.4f, 0.4f);
glm::vec3 eyeDirection = glm::vec3(0.0f, 0.0f, 0.0f);
GLboolean isLocal = true;
GLboolean twoSided = false;
int matIndex;

GLenum polygonMode = GL_FILL;

// glutKeyboardFunc is called below to set this function to handle
//		all "normal" key presses.
static void KeyPressFunc( unsigned char Key, int x, int y )
{
	switch ( Key ) {
	case 'R':
	case 'r':
		Key_r();
		break;
	case 's':
	case 'S':
		Key_s();
		break;
	case ' ':
		Key_space();
		break;
	case 'm':
		Key_m();
		break;
	case 27:	// Escape key
		exit(1);
	}
}

// glutSpecialFunc is called below to set this function to handle
//		all "special" key presses.  See glut.h for the names of
//		special keys.
static void SpecialKeyFunc( int Key, int x, int y )
{
	switch ( Key ) {
	case GLUT_KEY_UP:		
		Key_up();
		break;
	case GLUT_KEY_DOWN:
		Key_down();
		break;
	}
}

static void Key_r(void)
{
	if ( singleStep ) {			// If ending single step mode
		singleStep = GL_FALSE;
		spinMode = GL_TRUE;		// Restart animation
	}
	else {
		spinMode = !spinMode;	// Toggle animation on and off.
	}
}

static void Key_s(void)
{
	singleStep = GL_TRUE;
	spinMode = GL_TRUE;
}

static void Key_up(void)
{
    AnimateIncrement *= 2.0;			// Double the animation time step
}

static void Key_down(void)
{
    AnimateIncrement /= 2.0;			// Halve the animation time step
	
}

static void Key_space(void)
{
	sjencanje = !sjencanje;
}

static void Key_m(void)
{
	mode = !mode;
}

//void init_data(); // nasa funkcija za inicijalizaciju podataka

//*********************************************************************************
//	Glavni program.
//*********************************************************************************

int main(int argc, char ** argv)
{
	// Sljedeci blok sluzi kao bugfix koji je opisan gore
	#ifdef LINUX_UBUNTU_SEGFAULT
        //ss2
        int i=pthread_getconcurrency();        
	#endif

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 800, 480 );
	glutInit(&argc, argv);

	window = glutCreateWindow( "Solar System Demo" );
	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(Animate);
	glutKeyboardFunc( KeyPressFunc );
	glutSpecialFunc( SpecialKeyFunc );

	glewExperimental = GL_TRUE;
	glewInit();

	if(!init_data()) return 1;

	// Omogući uporabu Z-spremnika
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
    	return 0;
}

//*********************************************************************************
//	Funkcija u kojoj se radi inicijalizacija potrebnih VAO-a i VBO-ova. 
//*********************************************************************************

bool init_data()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	
	glShadeModel( GL_FLAT );
	glEnable( GL_DEPTH_TEST );
	
	// Stvori jedan VAO i njegov identifikator pohrani u vertexArrayID
	glGenVertexArrays(1, &vertexArrayID);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(vertexArrayID);

	// An array of 3 vectors which represents 3 spheres
	static const GLfloat patch[]={ 1.3f, 20.0f, 20.0f, 0.0f,
									0.4f, 15.0f, 15.0f, 0.0f,
									0.2f, 10.0f, 10.0f, 0.0f };

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(patch), patch, GL_STATIC_DRAW);

	// Podesi da se 0. atribut dohvaca iz vertex spremnika
	glVertexAttribPointer(
	   0,                  // attribute 0.
	   4,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);

	std::cout << "Going to load programs... " << std::endl << std::flush;

	programID = loadShaders("SimpleVertexShader.vert", "FragmentShader.fragmentshader", "TessCont.tesscontshader", "TessEval.tessevalshader");
	if(programID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

// light properties
	lights[0].isEnabled = true;
	lights[0].isPositional = true;
	lights[0].isSpot = false;
	lights[0].ambient = glm::vec3(0.0, 0.0, 0.0);
	lights[0].diffColor =  glm::vec3(1.0, 1.0, 1.0); 
	lights[0].specColor = glm::vec3(1.0, 1.0, 1.0);
	lights[0].position =  glm::vec3(0.0, 0.0, 0.0); 
	lights[0].halfVector = glm::vec3(0.0, 0.0, 0.0); 
	lights[0].coneDirection = glm::vec3(1.0, 0.0, 0.0);
	lights[0].spotCosCutoff = 0.0; 
	lights[0].spotExponent = 1.0;
	lights[0].constantAttenuation = 1.0;
	lights[0].linearAttenuation = 0.0;
	lights[0].quadraticAttenuation = 0.0;
	
	lights[1].isEnabled = false;
	lights[1].isPositional = true;
	lights[1].isSpot = false;
	lights[1].ambient = glm::vec3(0.0, 0.0, 0.0);
	lights[1].diffColor =  glm::vec3(1.0, 1.0, 1.0); 
	lights[1].specColor = glm::vec3(0.0, 0.0, 0.0);
	lights[1].position = glm::vec3(0.0, 0.0, 0.0); 
	lights[1].halfVector = glm::vec3(0.0, 0.0, 0.0);
	lights[1].coneDirection = glm::vec3(1.0, 0.0, 0.0);
	lights[1].spotCosCutoff = 0.0; 
	lights[1].spotExponent = 1.0;
	lights[1].constantAttenuation = 1.0;
	lights[1].linearAttenuation = 0.0;
	lights[1].quadraticAttenuation = 0.0;
	
	lights[2].isEnabled = false;
	lights[2].isPositional = true;
	lights[2].isSpot = false;
	lights[2].ambient = glm::vec3(0.0, 0.0, 0.0);
	lights[2].diffColor =  glm::vec3(1.0, 1.0, 1.0); 
	lights[2].specColor = glm::vec3(0.0, 0.0, 0.0);
	lights[2].position = glm::vec3(0.0, 0.0, 0.0); 
	lights[2].halfVector = glm::vec3(0.0, 0.0, 0.0); 
	lights[2].coneDirection = glm::vec3(1.0, 0.0, 0.0);
	lights[2].spotCosCutoff = 0.0; 
	lights[2].spotExponent = 1.0;
	lights[2].constantAttenuation = 1.0;
	lights[2].linearAttenuation = 3.0;
	lights[2].quadraticAttenuation = 0.0;
	
	//material properties
	material[0].materialColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
	material[0].emission = glm::vec3(0.7, 0.2, 0.2);
	material[0].ambient = glm::vec3(0.2, 0.2, 0.2); 
	material[0].diffuse = glm::vec3(1.0, 0.8, 0.0); 
	material[0].specular = glm::vec3(0.0, 0.0, 0.0);
	material[0].shininess = 1.0; 
	
	material[1].materialColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
	material[1].emission = glm::vec3(0.0, 0.0, 0.0);
	material[1].ambient = glm::vec3(0.2, 0.2, 0.2); 
	material[1].diffuse = glm::vec3(0.0, 0.0, 1.0); 
	material[1].specular = glm::vec3(0.0, 0.0, 0.0);
	material[1].shininess = 1.0; 
	
	material[2].materialColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
	material[2].emission = glm::vec3(0.0, 0.0, 1.0);
	material[2].ambient = glm::vec3(0.0, 0.0, 0.0); 
	material[2].diffuse =  glm::vec3(0.0, 0.0, 0.0); 
	material[2].specular = glm::vec3(0.0, 0.0, 0.0);
	material[2].shininess = 1.0; 
	
	material[3].materialColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
	material[3].emission = glm::vec3(0.0, 0.0, 0.0);
	material[3].ambient = glm::vec3(0.2, 0.2, 0.2); 
	material[3].diffuse = glm::vec3(0.8, 0.8, 0.8); 
	material[3].specular = glm::vec3(0.0, 0.0, 0.0);
	material[3].shininess = 1.0; 
	
	material[4].materialColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
	material[4].emission = glm::vec3(0.8, 0.8, 0.8);
	material[4].ambient =  glm::vec3(0.0, 0.0, 0.0); 
	material[4].diffuse =  glm::vec3(0.0, 0.0, 0.0); 
	material[4].specular = glm::vec3(0.0, 0.0, 0.0);
	material[4].shininess = 1.0; 
	
	// allocate memory for light buffer
	glGenBuffers(1, &mylightbuffer);
	
	// allocate memory for material buffer
	GLuint mymaterialbuffer; 
	glGenBuffers(1, &mymaterialbuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mymaterialbuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(material), material, GL_DYNAMIC_DRAW); 
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
 
	// bind material buffer to location 2
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, mymaterialbuffer);

	// Definiranje identifikatora za uniformne varijable  
	MVMatrixID = glGetUniformLocation(programID, "MVMatrix");
	NMatrixID = glGetUniformLocation(programID, "NMatrix");
	PMatrixID = glGetUniformLocation(programID, "PMatrix");
	AmbientalID = glGetUniformLocation(programID, "Ambiental");
	EyeDirectionID = glGetUniformLocation(programID, "EyeDirection");
	IsLocalID = glGetUniformLocation(programID,"IsLocal");
	TwoSidedID = glGetUniformLocation(programID,"TwoSided");
	MatIndexID = glGetUniformLocation(programID, "MatIndex");

	return true;
}

//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void Animate()
{
	glm::mat4 pomModel1,pomModel2,pomNorm1,pomNorm2;
	glm::mat4 model,normModel,pomMat;
	
	stoperica.setWaitTime( 1.0/fps );
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	if (spinMode) {
		// Update the animation state
        HourOfDay += AnimateIncrement;
        DayOfYear += AnimateIncrement/24.0;

        HourOfDay = HourOfDay - ((int)(HourOfDay/24))*24;
        DayOfYear = DayOfYear - ((int)(DayOfYear/365))*365;
		}
  
 	// Model matrix : 
 	model = glm::mat4(1.0f);
 	normModel = glm::mat4(1.0f);
 	model = glm::translate (model,glm::vec3(0.0f, 0.0f, -8.0f));
 	normModel = glm::rotate (normModel,glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
 	model = model * normModel;
 	
	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator vertexArrayID
	glBindVertexArray(vertexArrayID);

	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);

	// Zatraži da shaderima upravlja naš program čiji je identifikator programID
	glUseProgram(programID);
	
	glUniform3fv(AmbientalID, 1, &ambiental[0]);
	glUniform3fv(EyeDirectionID, 1, &eyeDirection[0]);
	glUniform1i(IsLocalID, isLocal);
	glUniform1i(TwoSidedID, twoSided);
	glUniformMatrix4fv(PMatrixID, 1, GL_FALSE, &projection[0][0]);
	
	lights[1].isEnabled = false;
	lights[2].isEnabled = false;
	if (!sjencanje) 
	{
		lights[0].isEnabled = false;		
	}
	else
	{
		lights[0].isEnabled = true;	
	}
	
	glBindBuffer(GL_UNIFORM_BUFFER, mylightbuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), lights, GL_DYNAMIC_DRAW); 
	
	// bind light buffer to location 1
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, mylightbuffer);
	
	if (mode) polygonMode = GL_FILL;
	else polygonMode = GL_LINE;
	glPolygonMode(GL_FRONT_AND_BACK,polygonMode); 
	
// Sunce!!!	

	glUniformMatrix4fv(MVMatrixID, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(NMatrixID, 1, GL_FALSE, &normModel[0][0]);
	matIndex = 0;
	glUniform1i(MatIndexID, matIndex);
	glPatchParameteri(GL_PATCH_VERTICES,1);
	glDrawArrays(GL_PATCHES,0, 1);
	
	lights[1].position = glm::vec3(model * glm::vec4(0.0, 0.0, 0.0, 1.0));
	lights[0].isEnabled = false;
	lights[1].isEnabled = true;
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), lights, GL_DYNAMIC_DRAW); 
	
// Zemlja položaj!!!

	pomMat = glm::rotate (glm::radians(360.0f*DayOfYear/365.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = model * pomMat;
	normModel = normModel * pomMat;
	model = glm::translate (model,glm::vec3(3.5f, 0.0f, 0.0f));
	pomModel1 = model;
	pomNorm1 = normModel;
	pomMat = glm::rotate (glm::radians(360.0f*HourOfDay/24.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = model * pomMat;
	normModel = normModel * pomMat;
	pomModel2 = model;
	pomNorm2 = normModel;
	
// Mjesec!!!

	model = pomModel1;
	normModel = pomNorm1;
	pomMat = glm::rotate (glm::radians(360.0f*12.0f*DayOfYear/365.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = model * pomMat;
	normModel = normModel * pomMat;
	model = glm::translate (model,glm::vec3(0.9f, 0.0f, 0.0f));
	glUniformMatrix4fv(MVMatrixID, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(NMatrixID, 1, GL_FALSE, &normModel[0][0]);

	lights[2].position = glm::vec3(model * glm::vec4(0.0, 0.0, 0.0, 1.0));
	if (sjencanje) {
		matIndex = 3;
		lights[2].isEnabled = true;
	}
	else {
		matIndex = 4;
		lights[1].isEnabled = false;
		lights[2].isEnabled = false;
	}	
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), lights, GL_DYNAMIC_DRAW); 
	glUniform1i(MatIndexID, matIndex);
	glDrawArrays(GL_PATCHES,2, 1);
	
// Zemlja crtanje!!!

	model = pomModel2;
	normModel = pomNorm2;
	glUniformMatrix4fv(MVMatrixID, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(NMatrixID, 1, GL_FALSE, &normModel[0][0]);
	if (sjencanje) {
		matIndex = 1;
		lights[2].isEnabled = true;
	}
	else {
		matIndex = 2;
		lights[1].isEnabled = false;
		lights[2].isEnabled = false;
	}
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), lights, GL_DYNAMIC_DRAW); 
	glUniform1i(MatIndexID, matIndex);
	glDrawArrays(GL_PATCHES,1, 1);
	
	// onemogući slanje atributa nula i jedan shaderu
	glDisableVertexAttribArray(0);
	
	glutSwapBuffers();
	if ( singleStep ) {
		spinMode = GL_FALSE;	// Trigger an automatic redraw for animation
	}

	stoperica.wait();
	glutPostRedisplay();
}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void resizeWindow(int w, int h)
{
	float aspectRatio;
	h = (h == 0) ? 1 : h;
	w = (w == 0) ? 1 : w;
	glViewport( 0, 0, w, h );	// View port uses whole window
	aspectRatio = (float)w/(float)h;
	
    projection = glm::perspective(glm::radians(60.0f), aspectRatio, 1.0f, 30.0f);
}



