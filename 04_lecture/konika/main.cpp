// Kompajliranje:
// g++ -o konika konika.cpp util.cpp util_t.cpp -lGLEW -lGL -lGLU -lglut -lpthread

#ifdef _WIN32
#include <windows.h>             //bit ce ukljuceno ako se koriste windows
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

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
GLuint sub_width = 500, sub_height = 500;

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay		();
void myReshape		(int width, int height);
void myMouseFunc	( int button, int state, int x, int y );
void Motion			(int x, int y);
void myKeyboardFunc	( unsigned char key, int x, int y );

GLfloat w[3];
GLuint vertexArrayID;
GLuint programID,programID_t;
GLuint MVPMatrixID,MVPMatrixID_t; 
GLuint ColorID,ColorID_t;
GLuint OrderID;
Shader s1,s2;
GLuint vertexbuffer;

const float Xmin = 0.0, Xmax = 1.0;
const float Ymin = 0.0, Ymax = 1.0;

glm::vec4 Tocke[3],Poligon[3];
int n=0;
GLdouble eps=0.03;
static const GLfloat color1[]={1.0f, 0.0f, 1.0f}, color2[]={0.0f, 1.0f, 1.0f}, color3[]={1.0f, 1.0f, 0.0f};

bool init_data(); // nasa funkcija za inicijalizaciju podataka

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
	
	for (int i=0; i<3; i++) {
		std::cout << "w(" << i << ")=";
		std::cin >> w[i];
	}

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(sub_width,sub_height);
	glutInitWindowPosition(20,60);
	glutInit(&argc, argv);

	window = glutCreateWindow("Glut OpenGL Prozor");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouseFunc);
	glutMotionFunc(Motion);
	glutKeyboardFunc(myKeyboardFunc);

	glewExperimental = GL_TRUE;
	glewInit();

	if(!init_data()) return 1;

	// Omogući uporabu Z-spremnika
	glEnable(GL_DEPTH_TEST);
	// Prihvaćaj one fragmente koji su bliže kameri u smjeru gledanja
	glDepthFunc(GL_LESS);

	glutMainLoop();
    	return 0;
}

void myKeyboardFunc( unsigned char key, int x, int y ){
	switch ( key ) {

	case 27:
		exit(1);

	}
}

void myMouseFunc( GLint button, GLint state, GLint x, GLint y )
{
    if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
		GLfloat xPos = GLfloat( x )/(GLfloat)sub_width ;
        GLfloat yPos = 1-GLfloat( y )/(GLfloat)sub_height; 

		for( int i=0; i<n; ++i ) 
			if( glm::length(glm::vec2(Poligon[i])-glm::vec2(xPos,yPos))<eps || n>=3 ) return;
		
		Poligon[n]=glm::vec4(xPos ,yPos, 0.0f, 1.0f);
		Tocke[n]=w[n]*Poligon[n];
		n++;

	}
 
       glutPostRedisplay();

}

void Motion(int x, int y)
{
	GLfloat xPos = GLfloat( x )/(GLfloat)sub_width ;
    GLfloat yPos = 1-GLfloat( y )/(GLfloat)sub_height; 

		
	for( int i=0; i<n; ++i ) 
		if( glm::length(glm::vec2(Poligon[i])-glm::vec2(xPos,yPos))<eps )
			{Poligon[i].x=xPos; Poligon[i].y=yPos; 
			 Tocke[i]=w[i]*Poligon[i]; break; }
				  
	glutPostRedisplay();

} 

void myKeyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
      exit(0);
      break;
   }
}

//*********************************************************************************
//	Funkcija u kojoj se radi inicijalizacija potrebnih VAO-a i VBO-ova. 
//*********************************************************************************

bool init_data()
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	
	// Stvori jedan VAO i njegov identifikator pohrani u vertexArrayID
	glGenVertexArrays(1, &vertexArrayID);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(vertexArrayID);
	
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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

	programID_t = s2.load_shaders({"SimpleVertexShader_t.vert", "SimpleFragmentShader.frag", "","TessCont.tesc", "TessEval.tese"});
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

 	// Projection matrix
 	glm::mat4 projection = glm::ortho(Xmin, Xmax, Ymin, Ymax, -1.0f, 1.0f); 
 
 	// Model matrix : an identity matrix (model will be at the origin)
 	glm::mat4 model = glm::mat4(1.0f);
 	// Our ModelView : multiplication of our 2 matrices
 	glm::mat4 mvp=projection * model;

	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator vertexArrayID
	glBindVertexArray(vertexArrayID);

	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);

	// Crtanje kontrolnog poligona
	glBufferData(GL_ARRAY_BUFFER, sizeof(Poligon), Poligon, GL_DYNAMIC_DRAW);	
	glUseProgram(programID);
	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniform3fv(ColorID,1,color2);
	glPointSize(6);	
	glDrawArrays(GL_POINTS, 0, n);	
	glUniform3fv(ColorID,1,color3);
	glLineWidth(1);	
	glDrawArrays(GL_LINE_STRIP, 0, n);
	
	// Crtanje konike
	glBufferData(GL_ARRAY_BUFFER, sizeof(Tocke), Tocke, GL_DYNAMIC_DRAW);	
	glUseProgram(programID_t);
	glUniformMatrix4fv(MVPMatrixID_t, 1, GL_FALSE, &mvp[0][0]);
	glUniform3fv(ColorID_t,1,color1);
	glPointSize(2);
	glPatchParameteri(GL_PATCH_VERTICES,3);
	if (n>2) {
		
		glDrawArrays(GL_PATCHES, 0, 3); }

	// onemogući slanje atributa nula shaderu
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void myReshape(int width, int height)
{
	sub_width = width;                      	// zapamti novu sirinu prozora
    sub_height = height;				// zapamti novu visinu prozora
    glViewport(0, 0, width, height);
}

