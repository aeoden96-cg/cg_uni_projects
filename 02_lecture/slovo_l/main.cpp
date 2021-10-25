// Kompajliranje:
// g++ -o Afine_trokut2 Afine_trokut2.cpp util.cpp -lGLEW -lGL -lGLU -lglut -lpthread

#ifdef _WIN32
#include <windows.h>             //bit ce ukljuceno ako se koriste windows
#endif

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
#include "util.hpp"

//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window; 
GLuint sub_width = 500, sub_height = 500;

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay		();
void myReshape		(int width, int height);
static void mySpecialKeyFunc( int Key, int x, int y );
bool init_data(); // nasa funkcija za inicijalizaciju podataka

float CurrentAngle = 0.0f;			// Angle in degrees
float AnimateStep = 1.0f;

GLuint VAO;
GLuint programID;
GLuint MVPMatrixID;
GLuint ColorID;

GLuint VBO_letter,vertexbuffer2;
static const GLfloat color1[]={1.0f, 1.0f, 0.0f, 1.0f}, color2[]={1.0f, 1.0f, 1.0f, 1.0f};
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 projection = glm::ortho(-5.0f,5.0f,-5.0f,5.0f,-1.0f,1.0f); // In world coordinates


std::stack<glm::mat4> mvstack;
GLint broj=0;

static void Key_up(void)
{
    glm::vec3 rot= glm::vec3 (0,1,0);
	++broj;
	mvstack.push(model);

	model = glm::rotate (model,glm::radians(-5.0f), rot);
	glutPostRedisplay(); 
}

static void Key_down(void)
{
	if (broj>0) {
		--broj;	
		model = mvstack.top();
		mvstack.pop();    
		glutPostRedisplay();}	

}

static void mySpecialKeyFunc( int Key, int x, int y )
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(sub_width,sub_height);
	glutInitWindowPosition(100,100);
	glutInit(&argc, argv);

	window = glutCreateWindow("Glut OpenGL Prozor");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutSpecialFunc( mySpecialKeyFunc );

	glewExperimental = GL_TRUE;
	glewInit();

	if(!init_data()) return 1;

	glutMainLoop();
    	return 0;
}

bool init_data()
{
	glEnable( GL_DEPTH_TEST );
	glPointSize(8);
	glLineWidth(1);

	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	static const GLfloat letter_coord[] = {
	   0.0f, 1.0f, 1.0f,
	   0.0f, -1.0f, 1.0f,
	   1.0f,  -1.0f, 1.0f,
       0.0f, -1.0f, 1.0f,
	};


    glGenBuffers(1, &VBO_letter);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_letter);
	glBufferData(GL_ARRAY_BUFFER, sizeof(letter_coord), letter_coord, GL_STATIC_DRAW);

    glVertexAttribPointer(
            0,                  // attribute 0.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );
    glEnableVertexAttribArray(0);

	std::cout << "Going to load programs... " << std::endl << std::flush;
	programID = loadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");
	if(programID==0) { std::cout << "Loading shaders error" << std::endl; return false;}



	// Get a handle for our uniforms for later when drawing...
	MVPMatrixID = glGetUniformLocation(programID, "MVP");
	ColorID = glGetUniformLocation(programID, "clr");

	return true;
}

void myDisplay()
{
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CurrentAngle =1;
    if ( CurrentAngle > 360.0 ) {
        CurrentAngle -= 360.0*floor(CurrentAngle/360.0);	// Don't allow overflow
    }

    glm::vec3 rot_axis= glm::vec3 (0, 1, 0);
    model = glm::rotate (model, (float) (CurrentAngle*M_PI/180.0), rot_axis);
        glm::mat4  mvp = projection * model;

    ////////////////////////// MAIN LOOP
	glUseProgram(programID);
    glBindVertexArray(VAO);

	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniform4fv(ColorID,1,color1);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

    glutPostRedisplay();
	glutSwapBuffers();
}

void myReshape(int width, int height)
{
	sub_width = width;                  // zapamti novu sirinu prozora
    sub_height = height;				// zapamti novu visinu prozora
    glViewport( 0, 0, sub_width, sub_height );
}
