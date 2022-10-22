// Kompajliranje:
// g++ -o SimpleNurbs SimpleNurbs.cpp util.cpp util_g.cpp -lGLEW -lGL -lGLU -lglut -lpthread

/*Bezierove krpice
 *
 * REZOLUCIJA MREZE
 * "U" povecava gustocu u U smjeru
 * "u" smanjuje gustocu u U smjeru
 * "V" povecava gustocu u V smjeru
 * "v" smanjuje gustocu u V smjeru
 *   
 * KONTROLA ANIMACIJE
 *   "a" ukljuci/iskljuci animaciju
 *   "s" jedan korak animacije
 *   "<-", "->" povecava/smanjuje brzinu rotacije oko y-osi
 *   slicno, strelica gore, dolje  povecava/smanjuje brzinu rotacije oko x-osi
 *   "r" - reset na pocetnu poziciju 
 *   "0" - ponistava rotaciju 		
 *
 *  OPENGL:
 *   "p" - mijenja zicani model/poligonalni
 * 	 "P" - Phongovo sjenčanje
 * 	 "G" - Gouradovo sjenčanje
 *   "f" - flat1 sjenčanje
 *	 "F" - flat2 sjenčanje
 *
 *
 **/

#ifdef _WIN32
#include <windows.h>             //bit ce ukljuceno ako se koriste windows
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "main.hpp"

#include <GL/glew.h>
#include <GL/freeglut.h>

// Ukljuci potporu za osnovne tipove glm-a: vektore i matrice
#include <glm/glm.hpp>

// Ukljuci funkcije za projekcije, transformacije pogleda i slicno
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

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
#include "Shader.h"


//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window; 
float aspectRatio;

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

GLuint vertexArrayID;
GLuint programPhongID,programGouradID,programFlat1ID,programFlat2ID;
GLuint programID;
GLuint MVPMatrixID,MVMatrixID,PMatrixID,nID,mID;
GLuint AmbientalID,EyeDirectionID,IsLocalID,ColorID,TwoSidedID,UID,VID;
GLuint MatIndexID;

Shader s1,s2,s3,s4;

GLuint vertexbuffer,mylightbuffer,mymaterialbuffer; 

// Kontrolne tocke za 4x4 Bezierovu krpicu (nema tocke u beskonacnosti)
const GLfloat circularStrip[4][4][4] = {
	{ {-2, -1, 0, 1}, { -0.6667, -0.3333, 1.3333, 0.3333},
							{ 0.6667, -0.3333, 1.3333, 0.3333}, { 2, -1, 0, 1 } },
	{ {-3, 0, 0, 1}, { -1, 0, 2, 0.3333 }, 
							{ 1, 0, 2, 0.3333}, { 3, 0, 0, 1}},
	{ {-1.5, 0.5, 0, 1}, {-0.5, 0.1667, 1, 0.3333}, 
							{0.5, 0.1667, 1, 0.3333}, {1.5, 0.5, 0, 1}},			
	{ {-2, 1, 0, 1}, { -0.6667,  0.3333, 1.3333, 0.3333},
							{ 0.6667,  0.3333, 1.3333, 0.3333}, { 2,  1, 0, 1 } }
};

// Varijable koje kontroliraju finocu poligonalne meze
int NumUs = 2;
int NumVs = 2;
	
GLenum polygonMode = GL_LINE;		

// Varijable koje kontroliraju animaciju
GLenum runMode = GL_TRUE;
GLenum singleStep = GL_FALSE;
float RotX = 0.0f;					// Pozicija za rotaciju oko x-osi
float RotY = 0.0f;					// Pozicija za rotaciju oko y-osi
float RotIncrementX = 0.0;			// Inkrement za rotaciju oko  x-osi
float RotIncrementY = 0.0;			// Inkrement za rotaciju oko  y-osi
const float RotIncFactor = 1.5;	    // Faktor povecanja brzine rotacije

GLint n=3,m=3; // Stupnjevi Bezierovih krivulja u krpici
glm::vec4 patchColor = glm::vec4(0.8f, 0.05f, 0.4f, 1.0f); // Boja krpice

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

LightProperties lights[2];
MaterialProperties material[1];
glm::vec3 ambiental = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 eyeDirection = glm::vec3(0.0f, 0.0f, 1.0f);
GLboolean isLocal = false;
GLboolean twoSided = true;
glm::mat4 matrixLights[2];
int matIndex=0;

// glutKeyboardFunc procesira dodire tipaka s "normalnim" kodovima
void myKeyboardFunc( unsigned char key, int x, int y ) 
{
	switch ( key ) {
	case 'a':
		if ( singleStep ) {	
			singleStep = GL_FALSE;
			runMode = GL_TRUE;
		}
		else {
			runMode = !runMode;
		}
		break;
	case 's':
		singleStep = GL_TRUE;
		runMode = GL_TRUE;
		break;
	case 27:	// Escape tipka
		exit(1);
	case 'r':	// Reset svega
		ResetAnimation();
		break;
	case '0':	// Inicijalizira brzine rotacije
		ZeroRotation();
		break;
	case 'P': // Phong
		programID = programPhongID;
		break;
	case 'G': // Gourad
		programID = programGouradID;
		break;
	case 'f':	// flat1 sjencanje
		programID = programFlat1ID;
		break;
	case 'F':	// flat2 sjencanje
		programID = programFlat2ID;
		break;
	case 'p':	// Zicani model/puni poligoni
		FillModeToggle();
		break;
	case 'u':	// dekrement  U parametra
		LessUs();
		break;
	case 'U':	// inkrement U parametra
		MoreUs();
		break;
	case 'v':	// dekrement  V parametra
		LessVs();
		break;
	case 'V':	// inkrement V parametra
		MoreVs();
		break;
	}
}

// glutSpecialFunc procesira dodire tipaka sa "specijalnim" kodovima
// v/ glut.h for za imena specijalnih kodova
//
void mySpecialKeyFunc( int key, int x, int y ) 
{
	switch ( key ) {
	case GLUT_KEY_UP:		
		// Povecava rotaciju prema gore, ili smanji prema dolje
		KeyUp();
		break;
	case GLUT_KEY_DOWN:
		// Povecava rotaciju prema dolje, ili smanji prema gore
		KeyDown();
		break;
	case GLUT_KEY_LEFT:
		// Povecava rotaciju prema lijevo, ili smanji prema desno
		KeyLeft();
		break;
	case GLUT_KEY_RIGHT:
		// Povecava rotaciju prema desno, ili smanji prema lijevo
		KeyRight();
		break;
	}
}

void KeyUp() {
    if ( RotIncrementX == 0.0 ) {
		RotIncrementX = -0.1;		// Inicijalno rotacija od desetine stupnja  
	}
	else if ( RotIncrementX < 0.0f) {
		RotIncrementX *= RotIncFactor;
	}
	else {
		RotIncrementX /= RotIncFactor;
	}	
}

void KeyDown() {
    if ( RotIncrementX == 0.0 ) {
		RotIncrementX = 0.1;		//Inicijalno rotacija od desetine stupnja
	}
	else if ( RotIncrementX > 0.0f) {
		RotIncrementX *= RotIncFactor;
	}
	else {
		RotIncrementX /= RotIncFactor;
	}	
}

void KeyLeft() {
    if ( RotIncrementY == 0.0 ) {
		RotIncrementY = -0.1;		// Inicijalno rotacija od desetine stupnja
	}
	else if ( RotIncrementY < 0.0) {
		RotIncrementY *= RotIncFactor;
	}
	else {
		RotIncrementY /= RotIncFactor;
	}	
}

void KeyRight()
{
    if ( RotIncrementY == 0.0 ) {
		RotIncrementY = 0.1;		// Inicijalno rotacija od desetine stupnja
	}
	else if ( RotIncrementY > 0.0) {
		RotIncrementY *= RotIncFactor;
	}
	else {
		RotIncrementY /= RotIncFactor;
	}	
}

// Reset pozicije i brzine rotacije
void ResetAnimation() {
	RotX = RotY = RotIncrementX = RotIncrementY = 0.0;
}

// Reset brzine rotacije
void ZeroRotation() {
	RotIncrementX = RotIncrementY = 0.0;
}

// line/fill modeli za poligone
void FillModeToggle() {
	if ( polygonMode == GL_LINE ) {
		polygonMode = GL_FILL;
	}
	else {
		polygonMode = GL_LINE;
	}
}

// Inkrement U parametara
void MoreUs() {
	NumUs++;
}

// Dekrement U parametara
void LessUs() {
	if (NumUs>4) {
		NumUs--;
	}
}

// Inkrement V parametara
void MoreVs() {
	NumVs++;
}

// Dekrement V parametara
void LessVs() {
	if (NumVs>4) {
		NumVs--;
	}
}

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

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(620,500);   
	glutInitWindowPosition(10,60);
	glutInit(&argc, argv);

	window = glutCreateWindow("Bezier-ova krpica");
	glutReshapeFunc( resizeWindow );
	glutDisplayFunc( updateScene );
	glutKeyboardFunc( myKeyboardFunc );
	glutSpecialFunc( mySpecialKeyFunc );

	glewExperimental = GL_TRUE;
	glewInit();

	if(!initRendering()) return 1;

	// Omogući uporabu Z-spremnika
	glEnable(GL_DEPTH_TEST);
	// Prihvaćaj one fragmente koji su bliže kameri u smjeru gledanja
	glDepthFunc(GL_LESS);

	glutMainLoop(); 
    	return 0;
}

//*********************************************************************************
//	Funkcija u kojoj se radi inicijalizacija potrebnih VAO-a i VBO-ova. 
//*********************************************************************************

bool initRendering()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	// Stvori jedan VAO i njegov identifikator pohrani u vertexArrayID
	glGenVertexArrays(1, &vertexArrayID);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(vertexArrayID);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(circularStrip), circularStrip, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
	   0,                  // attribute 0.
	   4,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	
	std::cout << "Going to load programs... " << std::endl << std::flush;

	programPhongID = s1.load_shaders({"VertexShader.vert", "FragmentShader.frag","", "TessCont1.tesc", "TessEval1.tese"});
	if(programPhongID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

	programGouradID = s2.load_shaders({"VertexShader.vert", "FragmentShader2.frag","", "TessCont1.tesc", "TessEval2.tese"});
	if(programGouradID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	} 
	
	programFlat1ID = s3.load_shaders({"VertexShader.vert", "FragmentShader3.frag", "GeometryShader.geom", "TessCont1.tesc", "TessEval2.tese"});
	if(programFlat1ID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	} 
	
	programFlat2ID = s4.load_shaders({"VertexShader.vert", "FragmentShader3.frag","GeometryShader2.geom", "TessCont1.tesc", "TessEval3.tese"});
	if(programFlat2ID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	} 

	
	programID = programPhongID; // Default

	// light properties
	lights[0].isEnabled = true;
	lights[0].isPositional = false;
	lights[0].isSpot = false;
	lights[0].ambient = glm::vec3(0.1, 0.1, 0.1);
	lights[0].diffColor =  glm::vec3(0.6, 0.6, 0.6); 
	lights[0].specColor = glm::vec3(1.0, 1.0, 1.0);
	lights[0].position = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)); 
	lights[0].halfVector = glm::normalize(lights[0].position+eyeDirection);
	lights[0].coneDirection = glm::vec3(1.0, 0.0, 0.0);
	lights[0].spotCosCutoff = 0.0; 
	lights[0].spotExponent = 1.0;
	lights[0].constantAttenuation = 1.0;
	lights[0].linearAttenuation = 0.0;
	lights[0].quadraticAttenuation = 0.0;
	
	lights[1].isEnabled = true;
	lights[1].isPositional = false;
	lights[1].isSpot = false;
	lights[1].ambient = glm::vec3(0.1, 0.1, 0.1);
	lights[1].diffColor =  glm::vec3(0.6, 0.6, 0.6); 
	lights[1].specColor = glm::vec3(1.0, 1.0, 1.0);
	lights[1].position = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f)); 
	lights[1].halfVector = glm::normalize(lights[1].position+eyeDirection); 
	lights[1].coneDirection = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f)); 
	lights[1].spotCosCutoff = 0.5; 
	lights[1].spotExponent = 10.0;
	lights[1].constantAttenuation = 1.0;
	lights[1].linearAttenuation = 0.0;
	lights[1].quadraticAttenuation = 0.0;
	
	// material properties
	material[0].materialColor = patchColor;
	material[0].emission = glm::vec3(0.0, 0.0, 0.0);
	material[0].ambient = glm::vec3(1.0, 1.0, 1.0); 
	material[0].diffuse = glm::vec3(1.0, 1.0, 1.0); 
	material[0].specular = glm::vec3(1.0, 1.0, 1.0);
	material[0].shininess = 50.0;
	
	// allocate memory for light buffer
	glGenBuffers(1, &mylightbuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mylightbuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), lights, GL_DYNAMIC_DRAW); 
	glBindBuffer(GL_UNIFORM_BUFFER, 0); 
 
	// bind light buffer to location 1
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, mylightbuffer);
	
	// allocate memory for material buffer
	glGenBuffers(1, &mymaterialbuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mymaterialbuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(material), material, GL_DYNAMIC_DRAW); 
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
 
	// bind material buffer to location 2
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, mymaterialbuffer);
		
	return true;
}

//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void updateScene()
{	
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	
 	glEnable(GL_DEPTH_TEST);

 	// Projection matrix : 
 	glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspectRatio, 1.0f, 30.0f); 
  
 	// Camera matrix
 	glm::mat4 view = glm::mat4(1.0f); 
  
 	// Model matrix :
 	glm::mat4 model = glm::mat4(1.0f);
 	model = glm::translate (model,glm::vec3(0.0f, 0.0f, -10.0f));
 	model = glm::rotate (model,glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
 	
 	// Update orijentacije ako je animacija u toku
	if ( runMode ) {
		RotY += RotIncrementY;
		if ( fabs(RotY)>360.0 ) {
			RotY -= 360.0*((int)(RotY/360.0));
		}
		RotX += RotIncrementX;
		if ( fabs(RotX)>360.0 ) {
			RotX -= 360.0*((int)(RotX/360.0));
		}
	}
 	// Inicijaliziraj orijentaciju
 	model = glm::rotate (model,glm::radians(RotX), glm::vec3(1.0f, 0.0f, 0.0f));
 	model = glm::rotate (model,glm::radians(RotY), glm::vec3(0.0f, 1.0f, 0.0f));
 	
 	// Our ModelView : multiplication of our 2 matrices
 	glm::mat4 mv = view * model;

	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator vertexArrayID
	glBindVertexArray(vertexArrayID);

	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);
	
	// Definiranje identifikatora za uniformne varijable
	MVMatrixID = glGetUniformLocation(programID, "MVMatrix");
	PMatrixID = glGetUniformLocation(programID, "PMatrix");
	nID = glGetUniformLocation(programID, "N");
	mID = glGetUniformLocation(programID, "M");
	AmbientalID = glGetUniformLocation(programID, "Ambiental");
	EyeDirectionID = glGetUniformLocation(programID, "EyeDirection");
	IsLocalID = glGetUniformLocation(programID,"IsLocal");
	TwoSidedID = glGetUniformLocation(programID,"TwoSided");
	MatIndexID = glGetUniformLocation(programID, "MatIndex");
	UID = glGetUniformLocation(programID,"U");
	VID = glGetUniformLocation(programID,"V");

	// Postavljanje vrijednosti za uniformne varijable
	glUseProgram(programID);
	glUniformMatrix4fv(MVMatrixID, 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix4fv(PMatrixID, 1, GL_FALSE, &projection[0][0]);
	glUniform1i(nID, n);
	glUniform1i(mID, m);
	glUniform3fv(AmbientalID, 1, &ambiental[0]);
	glUniform3fv(EyeDirectionID, 1, &eyeDirection[0]);
	glUniform1i(IsLocalID, isLocal);
	glUniform1i(TwoSidedID, twoSided);
	glUniform1i(MatIndexID, matIndex);
	glUniform1i(UID, NumUs);
	glUniform1i(VID, NumVs);
	
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);	// Odredi model za poligone
	glPatchParameteri(GL_PATCH_VERTICES,16);
	glDrawArrays(GL_PATCHES,0, 16);

	// onemogući slanje atributa nula shaderu
	glDisableVertexAttribArray(0);
	
	if ( singleStep ) {
		runMode = GL_FALSE;	// Trigger an automatic redraw for animation
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void resizeWindow(int width, int height)
{
	glViewport( 0, 0, width, height );
    aspectRatio = (float)width/(float)height; 
}
