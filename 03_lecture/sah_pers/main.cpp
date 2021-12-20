// Kompajliranje:
// g++ -o sah_pers2 sah_pers2.cpp util.cpp -lGLEW -lGL -lGLU -lglut -lpthread

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

//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window; 
GLuint sub_width = 600, sub_height = 600;

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay		();
void myReshape		(int width, int height);
void myMouse		(int button, int state, int x, int y);
void myKeyboard		(unsigned char theKey, int mouseX, int mouseY);

GLuint vertexArrayID;
GLuint programID;
GLuint MVPMatrixID;

glm::mat4 projection;
Shader s;

const GLint first[8]={0,18,36,54,72,90,108,126};
const GLint count[8]={18,18,18,18,18,18,18,18};
static const glm::vec3 w = glm::vec3(0.9f, 0.9f, 0.9f), b = glm::vec3(0.2f, 0.2f, 0.2f);
GLfloat fov, old_fov, aspectRatio;
int drag, drag_start;

double min(double a, double b)
{
	return (a < b) ? a : b;
}
double max(double a, double b)
{
	return (a > b) ? a : b;
}

void motionFunc(int x, int y)
{
	if (drag) {
		fov = min(179.0, max(1.0, old_fov + (y - drag_start)));
		std::cout << "fov=" << fov << std::endl;
		glutPostRedisplay();
	}
}

void mouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		drag = 1;
		drag_start = y;
		old_fov = fov;
	} else {
		drag = 0;
	}
}

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
	glutMotionFunc(motionFunc);
	glutMouseFunc(mouseFunc);

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
	int i,j,n,k;
	
	fov = 90;
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	
	// Stvori jedan VAO i njegov identifikator pohrani u vertexArrayID
	glGenVertexArrays(1, &vertexArrayID);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(vertexArrayID);

	static glm::vec2 g_vertex_buffer_data[144];

	n=0;
	for (i = 0; i < 8; i++)
		for (j = 0; j < 9; j++) {
				g_vertex_buffer_data[n]=glm::vec2((i+1)/4.0-1.0,j/4.0-1.0);
				g_vertex_buffer_data[n+1]=glm::vec2(i/4.0-1.0,j/4.0-1.0);
				n=n+2;
		}

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	static glm::vec3 g_color_buffer_data[144];
	
	n=0;
	for (i = 0; i < 8; i++)
		for (j = 0; j < 9; j++) {
			if ((i + j) % 2 == 0) 
				for (k = 0; k < 2; k++) 
					g_color_buffer_data[n+k]=b;
			else
				for (k = 0; k < 4; k++) 
					g_color_buffer_data[n+k]=w;
			n=n+2;
		}
		
	//This will identify our color buffer
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
	   2,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	
	// Podesi da se 1. atribut dohvaca iz color spremnika
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
	   1,                  // attribute 1.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	

	std::cout << "Going to load programs... " << std::endl << std::flush;

	programID = s.load_shaders({"SimpleVertexShader.vert", "SimpleFragmentShader.frag", "SimpleGeometryShader.geom" ,"",""});
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

 	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
 	projection = glm::perspective(glm::radians(fov), aspectRatio, 1.0f, 30.0f);
  
 	// Camera matrix
 	glm::mat4 view = glm::lookAt(
	    glm::vec3(-1,-2,-2), // Camera is at (-1,-2,-2), in World Space
	    glm::vec3(0,0,0), // and looks at the origin
	    glm::vec3(0,1,0)  // Head is up 
 	); 
  
 	// Model matrix : an identity matrix (model will be at the origin)
 	glm::mat4 model = glm::mat4(1.0f);
 	// Our ModelViewProjection : multiplication of our 3 matrices
 	glm::mat4 mvp = projection * view * model; // Kasnije se mnozi matrica puta tocka - model matrica mora biti najbliza tocki

	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator vertexArrayID
	glBindVertexArray(vertexArrayID);

	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);
	// omogući slanje atributa jedan shaderu - pod indeks 1 u init smo povezali boje vrhova (x,y,z)
	glEnableVertexAttribArray(1);

	// Zatraži da shaderima upravlja naš program čiji je identifikator programID
    s.use();

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);
	
	// Zatraži crtanje toka vrhova pri čemu ih se interpretira kao TRIANGLE_STRIP
	glMultiDrawArrays(GL_TRIANGLE_STRIP,first,count,8);
	
	// onemogući slanje atributa nula i jedan shaderu
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutSwapBuffers();
}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void myReshape(int width, int height)
{
	aspectRatio=(float)width/(float)height;
	glViewport(0, 0, width, height); 
	projection = glm::perspective(glm::radians(fov), aspectRatio, 1.0f, 30.0f);
}

