// Kompajliranje:
// g++ -o SimpleAnim SimpleAnim.cpp util.cpp -lGLEW -lGL -lGLU -lglut -lpthread

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
#include "util.hpp"
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

glm::mat4 projection; 

int RunMode = 1;		// Used as a boolean (1 or 0) for "on" and "off"

// The next global variable controls the animation's state and speed.
float CurrentAngle = 0.0f;			// Angle in degrees
float AnimateStep = 1.0f;			// Rotation step per update

// These variables set the dimensions of the rectanglar region we wish to view.
const double Xmin = 0.0, Xmax = 3.0;
const double Ymin = 0.0, Ymax = 3.0;

bool init_data(); // nasa funkcija za inicijalizaciju podataka

// glutKeyboardFunc is called below to set this function to handle
//		all "normal" key presses.
void myKeyboardFunc( unsigned char key, int x, int y )
{
	switch ( key ) {
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
void mySpecialKeyFunc( int key, int x, int y )
{
	switch ( key ) {
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
	glutInitWindowSize(sub_width,sub_height);
	glutInitWindowPosition(100,100);
	glutInit(&argc, argv);

	window = glutCreateWindow("SimpleAnim" );
	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc( myKeyboardFunc );			// Handles "normal" ascii symbols
	glutSpecialFunc( mySpecialKeyFunc );		// Handles "special" keyboard keys

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
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

	// Antialijasiranje poligona, ovisno o implementaciji
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_POLYGON_SMOOTH, GL_DONT_CARE);
	
	// Stvori jedan VAO i njegov identifikator pohrani u VAO
	glGenVertexArrays(1, &VAO);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(VAO);

	// An array of 9 vectors which represents 9 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		-1,-1,0,
        0,-1,0,
        -0.5,-0.5,0,

        0,0,0,
        -0.5,-0.5,0,
        0,-1,0,

        0,0,0,
        0.5,-0.5,0,
        0,-1,0,

        0,-1,0,
        1,0,0,
        1,-1,0,

        0,0,0,
        -1,1,0,
        -1,-1,0,

        0,0,0,
        -1,1,0,
        1,1,0,

        0,0,0,
        0.5,0.5,0,
        0.5,-0.5,0,





	};


	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// An array of 9 rgbs which represents colors for 9 vertices
	static const GLfloat g_color_buffer_data[] = {
	   1.0f, 0.0f, 0.0f, 1.0f,
       1.0f, 0.0f, 0.0f, 1.0f,
       1.0f, 0.0f, 0.0f, 1.0f,
	   1.0f, 1.0f, 0.0f, 1.0f,
       1.0f, 1.0f, 0.0f, 1.0f,
       1.0f, 1.0f, 0.0f, 1.0f,
       1.0f, 1.0f, 0.0f, 1.0f,
       1.0f, 1.0f, 0.0f, 1.0f,
       1.0f, 1.0f, 0.0f, 1.0f,
       0.0f, 1.0f, 0.0f, 1.0f,
       0.0f, 1.0f, 0.0f, 1.0f,
       0.0f, 1.0f, 0.0f, 1.0f,
       0.5f, 1.0f, 0.0f, 1.0f,
       0.5f, 1.0f, 0.0f, 1.0f,
       0.5f, 1.0f, 0.0f, 1.0f,
       1,    0.5f, 0.0f, 1.0f,
       1,    0.5f, 0.0f, 1.0f,
       1,    0.5f, 0.0f, 1.0f,
       0,    1.0f, 1.0f, 1.0f,
       0,    1.0f, 1.0f, 1.0f,
       0,    1.0f, 1.0f, 1.0f,






	};


	// This will identify our color buffer
	GLuint colorbuffer;
	// Generate 1 buffer, put the resulting identifier in colorbuffer
	glGenBuffers(1, &colorbuffer);
	// The following commands will talk about our 'colorbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW); 

	// Podesi da se 0. atribut dohvaca iz vertex spremnika
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
	   0,                  // attribute 0.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	); 

	// Podesi da se 1. atribut dohvaca iz color spremnika
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
	   1,                  // attribute 1.
	   4,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	); 

	std::cout << "Going to load programs... " << std::endl << std::flush;

	programID = loadShaders("SimpleVertexShader.vert", "frag.frag");
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
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	if (RunMode==1) {
		// Calculate animation parameters
        CurrentAngle += AnimateStep;
		if ( CurrentAngle > 360.0 ) {
			CurrentAngle -= 360.0*floor(CurrentAngle/360.0);	// Don't allow overflow
		}
	}

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glUseProgram(programID);

    glDrawArrays(GL_TRIANGLES,0,21);

//    glDrawArrays(GL_LINE_LOOP,0,8);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	if ( RunMode==1 ) {
		glutPostRedisplay();	// Trigger an automatic redraw for animation
	}

	glutSwapBuffers();
}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void resizeWindow(int w, int h)
{
	double scale, center;
	double windowXmin, windowXmax, windowYmin, windowYmax;

	// Define the portion of the window used for OpenGL rendering.
	glViewport( 0, 0, w, h );	// View port uses whole window

	// Set up the projection view matrix: orthographic projection
	// Determine the min and max values for x and y that should appear in the window.
	// The complication is that the aspect ratio of the window may not match the
	//		aspect ratio of the scene we want to view.
	w = (w==0) ? 1 : w;
	h = (h==0) ? 1 : h;
	if ( (Xmax-Xmin)/w < (Ymax-Ymin)/h ) {
		scale = ((Ymax-Ymin)/h)/((Xmax-Xmin)/w);
		center = (Xmax+Xmin)/2;
		windowXmin = center - (center-Xmin)*scale;
		windowXmax = center + (Xmax-center)*scale;
		windowYmin = Ymin;
		windowYmax = Ymax;
	}
	else {
		scale = ((Xmax-Xmin)/w)/((Ymax-Ymin)/h);
		center = (Ymax+Ymin)/2;
		windowYmin = center - (center-Ymin)*scale;
		windowYmax = center + (Ymax-center)*scale;
		windowXmin = Xmin;
		windowXmax = Xmax;
	}
	
	// Now that we know the max & min values for x & y 
	//		that should be visible in the window,
	//		we set up the orthographic projection.
    projection = glm::ortho(
            (float) windowXmin,
            (float) windowXmax,
            (float) windowYmin,
            (float) windowYmax,
            -1.0f,
            1.0f);

}



