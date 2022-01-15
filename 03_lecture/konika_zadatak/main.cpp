// Kompajliranje:
// g++ -o torus torus.cpp util_t.cpp -lGLEW -lGL -lGLU -lglut -lpthread

#ifdef _WIN32
#include <windows.h>             //bit ce ukljuceno ako se koriste windows
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

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
#include "util/Shader.h"
#include "main.hpp"

//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window; 
GLuint sub_width = 500, sub_height = 500; 

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

GLuint VAO;
GLuint programID;
GLuint MVPMatrixID;
GLuint ColorID;
GLuint DetailID,ScaleID;


glm::mat4 projection; 

static int RunMode = 1;

static GLenum spinMode = GL_TRUE;
static GLenum singleStep = GL_FALSE;

// The next global variable controls the animation's state and speed.
float CurrentAngle = 0.0f;			// Angle in degrees
float AnimateStep = 3.0f;			// Rotation step per update

// These variables set the dimensions of the rectanglar region we wish to view.
const double Xmin = -5.0, Xmax = 5.0;
const double Ymin = -5.0, Ymax = 5.0;
Shader s;
// glutKeyboardFunc is called below to set this function to handle
//		all "normal" key presses.
static void KeyPressFunc( unsigned char Key, int x, int y )
{
	switch ( Key ) {
	case 'r':
		RunMode = 1-RunMode;		// Toggle to opposite value
		if ( RunMode==1 ) {
			glutPostRedisplay();
		}
		break;
	case 's':
		RunMode = 1;
		myDisplay();
		RunMode = 0;
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
		if ( AnimateStep < 1.0e3) {			// Avoid overflow problems
			AnimateStep *= sqrt(2.0);		// Increase the angle increment
		}
		break;
	case GLUT_KEY_DOWN:
		if (AnimateStep>1.0e-6) {		// Avoid underflow problems.
			AnimateStep /= sqrt(2.0);	// Decrease the angle increment
		}
		break;
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

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowPosition( 10, 60 );
    glutInitWindowSize( 360, 360 );
	glutInit(&argc, argv);

	window = glutCreateWindow( "Torus" );
	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(myDisplay);
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
	
	// Stvori jedan VAO i njegov identifikator pohrani u VAO
	glGenVertexArrays(1, &VAO);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(VAO);

	// An array of 1 vector which represents torus
	static const GLfloat patch[]={ 2.0f, 25.0f, 0.4f, 10.0f};

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

	programID = s.load_shaders({"SimpleVertexShader.vert",
                            "SimpleFragmentShader.frag","",
                            "TessCont.tesc",
                            "TessEval.tese"});
	if(programID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

	// Get a handle for our "MVP" uniform for later when drawing...
	MVPMatrixID = glGetUniformLocation(programID, "MVP");

	return true;
}

//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void myDisplay()
{	
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
	if (RunMode==1) {
		// Calculate animation parameters
        CurrentAngle += AnimateStep;
		if ( CurrentAngle > 360.0 ) {
			CurrentAngle -= 360.0*floor(CurrentAngle/360.0);	// Don't allow overflow
		}
	}
   
 	// Model matrix : 
 	glm::mat4 model = glm::mat4(1.0f);
 	model = glm::translate (model,glm::vec3(0.0f, 0.0f, -8.0f));
 	model = glm::translate (model,glm::vec3(-0.5f, 0.5f, 0.0f));
 	//model = glm::rotate (model,glm::radians(CurrentAngle), glm::vec3(1.0f, 1.0f, 0.0f));
 	model = glm::translate (model,glm::vec3(0.5f, -0.5f, 0.0f));
 	// Our ModelViewProjection : multiplication of our 2 matrices
 	glm::mat4 mvp = projection * model; // Kasnije se mnozi matrica puta tocka - model matrica mora biti najbliza tocki
	
	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator VAO
	glBindVertexArray(VAO);

	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);
	// Zatraži da shaderima upravlja naš program čiji je identifikator programID
	glUseProgram(programID);
	
	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);
	// crtanje torusa
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glPatchParameteri(GL_PATCH_VERTICES,1);
	glDrawArrays(GL_PATCHES,0, 1);

	// onemogući slanje atributa nula i jedan shaderu
	glDisableVertexAttribArray(0);
	
	if ( singleStep ) {
		spinMode = GL_FALSE;	// Trigger an automatic redraw for animation
	}

	glutSwapBuffers();
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



