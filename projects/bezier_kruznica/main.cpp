// Kompajliranje:
// g++ -o bezier_kruznica bezier_kruznica.cpp util.cpp util_t.cpp -lGLEW -lGL -lGLU -lglut -lpthread

#ifdef _WIN32
#include <windows.h>             //bit ce ukljuceno ako se koriste windows
#endif

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
#include "Shader.h"

#define N 500
#define T 4 

using namespace std;


//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window; 
GLuint sub_width = 500, sub_height = 500;

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************
Shader s,s1;
void myDisplay		();
void myReshape		(int width, int height);
void myMouseFunc	( int button, int state, int x, int y );
void myKeyboardFunc	( unsigned char key, int x, int y );

GLuint vertexArrayID;
GLuint programID,programID_t;
GLuint MVPMatrixID,MVMatrixID_t,PMatrixID_t; 
GLuint ColorID_t;

GLuint vertexbuffer[2];
GLuint vertexbuffer1,vertexbuffer2,colorbuffer;

float Xmin = -3.0, Xmax = 3.0;
float Ymin = -3.0, Ymax = 3.0;

int kruznica=0;

GLfloat ctrlpoints[6][4] = {
	{   0,   1,   0,   1}, 
        {   1,   0,   0,   0}, 
	{   0,  -1,   0,   1}, 
	{   0,   1,   0,   1}, 
        {  -1,   0,   0,   0}, 
	{   0,  -1,   0,   1}, 
};

GLfloat ctrlpoints2[6][4];

static const GLfloat color[]={1.0f, 1.0f, 0.0f};

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

void myKeyboardFunc( unsigned char key, int x, int y )
{
   switch (key) {
      case 27:
      exit(0);
      break;
      
      case ' ':
      kruznica=1-kruznica;
      glutPostRedisplay();
      break;
   }
}


//*********************************************************************************
//	Funkcija u kojoj se radi inicijalizacija potrebnih VAO-a i VBO-ova. 
//*********************************************************************************

bool init_data()
{
	//GLfloat color1[6][4];
	glm::vec4 color1[6];
	
	glClearColor (0.3, 0.3, 0.3, 0.0);
	
	// Stvori jedan VAO i njegov identifikator pohrani u vertexArrayID
	glGenVertexArrays(1, &vertexArrayID);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(vertexArrayID);
	
	// Generate 2 buffers, put the resulting identifier in vertexbuffer
	glGenBuffers(2, vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(ctrlpoints), ctrlpoints, GL_STATIC_DRAW);
	
	// Generate 1 buffer, put the resulting identifier in colorbuffer
	glGenBuffers(1, &colorbuffer);
	// The following commands will talk about our 'colorbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	
	for (int i=0; i<6; i++) {
		if (ctrlpoints[i][3]==0) {
			color1[i]=glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else {
			color1[i]=glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); 
		}
	}
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(color1), color1, GL_STATIC_DRAW);

	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
	// Give our vertices to OpenGL.
	
	for (int i=0; i<6; i++) {
		for (int j=0; j<4; j++) 
			ctrlpoints2[i][j]=ctrlpoints[i][j];
		if (ctrlpoints[i][3]==0.0) ctrlpoints2[i][3]=1.0;
	}
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(ctrlpoints2), ctrlpoints2, GL_STATIC_DRAW);

	std::cout << "Going to load programs... " << std::endl << std::flush;

	programID = s.load_shaders( {"SimpleVertexShader.vert", "SimpleFragmentShader.frag","","",""});
	if(programID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}  

	programID_t = s1.load_shaders({"SimpleVertexShader1.vert", "SimpleFragmentShader1.frag","", "TessCont.tesc", "TessEval.tese"});
	if(programID_t==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

	// Get a handle for our uniforms for later when drawing...
	MVPMatrixID = glGetUniformLocation(programID, "MVP");
	MVMatrixID_t = glGetUniformLocation(programID_t, "MV");
	PMatrixID_t = glGetUniformLocation(programID_t, "PP");
	ColorID_t = glGetUniformLocation(programID_t, "clr");

	return true;
}

//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void myDisplay()
{
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 	// Projection matrix : 
 	glm::mat4 projection = glm::ortho(Xmin, Xmax, Ymin, Ymax, -3.0f, 3.0f); 
  
 	// Model matrix : an identity matrix (model will be at the origin)
 	glm::mat4 model = glm::mat4(1.0f);
 	// Our ModelView : multiplication of our 2 matrices
 	glm::mat4 mvp=projection * model;

	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator vertexArrayID
	glBindVertexArray(vertexArrayID);
	
	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
	glVertexAttribPointer(
	   0,                  // attribute 0.
	   4,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	
	// Crtanje polukružnica
//	glUseProgram(programID_t);
    s1.use();
	glUniformMatrix4fv(MVMatrixID_t, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(PMatrixID_t, 1, GL_FALSE, &projection[0][0]);
	glUniform3fv(ColorID_t,1,color);
	glPatchParameteri(GL_PATCH_VERTICES,3);
	glDrawArrays(GL_PATCHES, 0, 3);
	if (kruznica==1) glDrawArrays(GL_PATCHES, 3, 3); // Ako je kruznica==1 crta se i lijeva polukružnica
	
	// Crtanje kontronih točaka (točke u beskonačnosti crtane sa w=1 i obojane crvenom bojom)
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
	glVertexAttribPointer(
	   0,                  // attribute 0.
	   4,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
	   1,                  // attribute 1.
	   4,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	
//	glUseProgram(programID);
    s.use();
	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);
	glPointSize(6);	
	glDrawArrays(GL_POINTS, 0, 3);
	if (kruznica==1) glDrawArrays(GL_POINTS, 3, 3); // Ako je kruznica==1 crtaju se i kontrolne točke lijeve kružnice

	// onemogući slanje atributa nula shaderu
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutSwapBuffers();
}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void myReshape(int width, int height)
{
    glViewport(0, 0, width, height);
    if (width <= height) {
		Xmin=-3.0;
		Xmax=3.0;
		Ymin=-3.0*(GLfloat)height/(GLfloat)width;
		Ymax= 3.0*(GLfloat)height/(GLfloat)width;
	}
	else {
		Xmin=-3.0*(GLfloat)width/(GLfloat)height;
		Xmax=3.0*(GLfloat)width/(GLfloat)height;
		Ymin=-3.0;
		Ymax=3.0;
	}
}

