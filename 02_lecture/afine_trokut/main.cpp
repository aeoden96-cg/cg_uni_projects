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

GLuint VAO;
GLuint programID;
GLuint MVPMatrixID;
GLuint ColorID;

GLuint VBO_letter,vertexbuffer2;
static const GLfloat color1[]={1.0f, 1.0f, 0.0f, 1.0f}, color2[]={1.0f, 1.0f, 1.0f, 1.0f};
glm::mat4 model = glm::mat4(1.0f);
std::stack<glm::mat4> mvstack;
GLint broj=0;

static void Key_up(void)
{
	++broj;
	mvstack.push(model);
	model = glm::translate (model,glm::vec3(0.3f, 1.0f, 0.0f));
	model = glm::rotate (model,glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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
	glutSpecialFunc( mySpecialKeyFunc );

	glewExperimental = GL_TRUE;
	glewInit();

	if(!init_data()) return 1;

	glutMainLoop();
    	return 0;
}

//*********************************************************************************
//	Funkcija u kojoj se radi inicijalizacija potrebnih VAO-a i VBO-ova. 
//*********************************************************************************

bool init_data()
{
	glEnable( GL_DEPTH_TEST );
	glPointSize(8);
	glLineWidth(1);
	
	// Antialijasiranje poligona, ovisno o implementaciji
	
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_POLYGON_SMOOTH, GL_DONT_CARE);
	
	// Stvori jedan VAO i njegov identifikator pohrani u VAO
	glGenVertexArrays(1, &VAO);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(VAO);

	// An array of 3 vectors which represents 3 vertices
	static const GLfloat triangle[] = {
	   1.0f, -1.0f, 1.0f,
	   0.0f, 1.0f, 1.0f,
	   -1.0f,  -1.0f, 1.0f,
	};
	
	// Točke koje definiraju koordinatne osi
	static const GLfloat coord[] = {
	   -5.0f, 0.0f, 0.0f,
	   5.0f, 0.0f, 0.0f,
	   0.0f,  -5.0f, 0.0f,
	   0.0f,  5.0f, 0.0f,
	};

	// Generate 1 buffer, put the resulting identifier in VBO_letter
	glGenBuffers(1, &VBO_letter);
	// The following commands will talk about our 'VBO_letter' buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO_letter);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer2
	glGenBuffers(1, &vertexbuffer2);
	// The following commands will talk about our 'vertexbuffer2' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(coord), coord, GL_STATIC_DRAW);

	std::cout << "Going to load programs... " << std::endl << std::flush;

	programID = loadShaders("SimpleVertexShader.vert", "frag.frag");
	if(programID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

	// Get a handle for our uniforms for later when drawing...
	MVPMatrixID = glGetUniformLocation(programID, "MVP");
	ColorID = glGetUniformLocation(programID, "clr");

	return true;
}

//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void myDisplay()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
 	// For an ortho camera :
 	glm::mat4 projection = glm::ortho(-5.0f,5.0f,-5.0f,5.0f,-1.0f,1.0f); // In world coordinates
  
 	// Model matrix : an identity matrix (model will be at the origin)
 	glm::mat4 model1 = glm::mat4(1.0f);

	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator VAO
	glBindVertexArray(VAO);

	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);
	
	// Zatraži da shaderima upravlja naš program čiji je identifikator programID
	glUseProgram(programID);
	
	// Crtanje koordinatnih osi
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glVertexAttribPointer(
	   0,                  // attribute 0.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	); 
	glm::mat4 mvp = projection * model1;
	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniform4fv(ColorID,1,color2);
	glDrawArrays(GL_LINES, 0, 4); 
	
	// Crtanje trokuta
	glBindBuffer(GL_ARRAY_BUFFER, VBO_letter);
	glVertexAttribPointer(
	   0,                  // attribute 0.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	
	mvp = projection * model;
	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniform4fv(ColorID,1,color1);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// onemogući slanje atributa nula shaderu
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void myReshape(int width, int height)
{
	sub_width = width;                  // zapamti novu sirinu prozora
    sub_height = height;				// zapamti novu visinu prozora
    glViewport( 0, 0, sub_width, sub_height );
}
