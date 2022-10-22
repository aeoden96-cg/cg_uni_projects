// Kompajliranje:
// g++ -o sv sv.cpp util.cpp util_t.cpp -lGLEW -lGL -lGLU -lglut -lpthread

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
#include "main.hpp"

//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window; 
GLint Width = 1000, Height = 1000; 

GLuint VAO;
GLuint programID,programID_t;
GLuint MVPMatrixID,MVPMatrixID_t;
GLuint ColorID,ColorID_t;
Shader s1,s2;
//#define MaxNumPts 64
glm::mat4 projection; 

// These three variables control the animation's state and speed.
static float broj = 0.0;
static float AnimateIncrement = 24.0;  // Time step for animation (hours)
static GLenum spinMode = GL_TRUE;
static GLenum singleStep = GL_FALSE;

static const GLfloat colors[]={1.0f, 1.0f, 0.0f}, coloro[]={1.0f, 1.0f, 1.0f}; 


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
	glutInitWindowPosition( 20, 60 );
    glutInitWindowSize( Width, Height );
	glutInit(&argc, argv);

	window = glutCreateWindow( "Glut OpenGL Prozor" );
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

	// An array of 12 vectors which represents 12 vertices
	static const GLfloat patch[]={150.0f, 14.0f, 15.0f, 0.0f, 
								  0.0f, 200.0f, 0.0f, 1.0f,
								  0.0f, -200.0f, 0.0f, 1.0f};

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

	programID = s1.load_shaders({"SimpleVertexShader.vert", "SimpleFragmentShader.frag","","",""});
	if(programID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}
	programID_t = s2.load_shaders({"SimpleVertexShader_t.vert", "SimpleFragmentShader.frag","", "TessCont.tesc", "TessEval.tese"});
	if(programID_t==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

	// Get a handle for our uniforms for later when drawing...
	MVPMatrixID = glGetUniformLocation(programID, "MVP");
	ColorID = glGetUniformLocation(programID, "clr");
	MVPMatrixID_t = glGetUniformLocation(programID_t, "MVP");
	ColorID_t = glGetUniformLocation(programID_t, "clr");

	return true;
}

//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void myDisplay()
{
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	if (spinMode) {
		broj += AnimateIncrement/24.0;
        broj = broj - ((int)(broj/365))*365;
		}
  

 	// Model matrix : 
 	glm::mat4 model = glm::mat4(1.0f);
 	model = glm::translate (model,glm::vec3(Width/2, Height/2, 0.0f));
 	model = glm::rotate (model,glm::radians(360.0f*broj/365.0f), glm::vec3(0.0f, 1.0f, 0.0f));
 	model = glm::rotate (model,glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
 	// Our ModelViewProjection : multiplication of our 2 matrices
 	glm::mat4 mvp = projection * model; 
	
	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator VAO
	glBindVertexArray(VAO);

	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);

	// Zatraži da shaderima upravlja naš program čiji je identifikator programID
	glUseProgram(programID_t);

	
// Sfera!!!	

	glUniformMatrix4fv(MVPMatrixID_t, 1, GL_FALSE, &mvp[0][0]);
	glUniform3fv(ColorID_t,1,colors);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); 
	glPatchParameteri(GL_PATCH_VERTICES,1);
	glDrawArrays(GL_PATCHES,0, 1);
	
// Os!!!

	glUseProgram(programID);
	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniform3fv(ColorID,1,coloro);
	glDrawArrays(GL_LINES,1, 2);  // novi vertex shader
		
	// onemogući slanje atributa nula shaderu
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
	Width = w;
	Height = h;
	h = (h == 0) ? 1 : h;
	w = (w == 0) ? 1 : w;
	glViewport( 0, 0, w, h );	// View port uses whole window
	
    projection = glm::ortho(0.0f,(float) w,0.0f,(float) h,-500.0f,500.0f);
}



