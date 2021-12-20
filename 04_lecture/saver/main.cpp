// Kompajliranje:
// g++ -o saver saver.cpp util.cpp util_t.cpp -lGLEW -lGL -lGLU -lglut -lpthread


/*
Ovaj program je imitacija Beziers screen savera.
N je broj krivulja koje program crta (velicina traga
koji krivulja ostavlja).
T je broj kontrolnih tocaka svake od krivulja.

Program bi trebalo doraditi tako da brzina kojom
se krivulje iscrtavaju ne ovisi o racunalu na kojem
se program izvodi (zadati fiksni fps).
*/

#ifdef _WIN32
#include <windows.h>             //bit ce ukljuceno ako se koriste windows
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

/* #include <cmath>
#include <cstdlib>
#include <ctime> */

#include <time.h>

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

#define N 500
#define T 4 

using namespace std;

struct Clock
{
	void setWaitTime( float seconds ){ end = clock() + seconds*CLOCKS_PER_SEC;	}
	void wait( void ){ for( ; clock() < end; ) ;	}
	float end;
} stoperica;

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
void myKeyboardFunc	( unsigned char key, int x, int y );
Shader s;
GLuint vertexArrayID;
GLuint programID_t;
GLuint MVMatrixID_t,PMatrixID_t; 
GLuint ColorID_t;

GLuint vertexbuffer;

const float Xmin = 0.0, Xmax = 1.0;
const float Ymin = 0.0, Ymax = 1.0;

const int fps = 40;
int n=0;
glm::vec3 tocke[N][T], boja;

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

	std::cout << "Going to load programs... " << std::endl << std::flush;

	programID_t = s.load_shaders({
        "SimpleVertexShader1.vert",
        "SimpleFragmentShader.frag",
        "",
        "TessCont.tesc",
        "TessEval.tese"});

	if(programID_t==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

	// Get a handle for our uniforms for later when drawing...
	MVMatrixID_t = glGetUniformLocation(programID_t, "MV");
	PMatrixID_t = glGetUniformLocation(programID_t, "PP");
	ColorID_t = glGetUniformLocation(programID_t, "clr");
	
	srand(time(NULL));
	// za inicijalnu boju odabiremo neku svjetliju
	boja=glm::vec3((float)(rand()%1000)/1000.f,(float)(rand()%1000)/1000.f,(float)(rand()%1000)/1000.f);

	return true;
}

//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void myDisplay()
{
	glm::vec3 bojan;

	stoperica.setWaitTime( 1.0/fps );

 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 	// Projection matrix :
 	glm::mat4 projection = glm::ortho(0.0f, (float) sub_width, 0.0f, (float) sub_height, (float) -N, (float) N); 
  
 	// Model matrix : an identity matrix (model will be at the origin)
 	glm::mat4 model = glm::mat4(1.0f);

	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator vertexArrayID
	glBindVertexArray(vertexArrayID);

	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);
	
	if (n == 0)
	{
		n=1;
		for (int i=0; i<T; ++i)
			tocke[0][i]=glm::vec3((float)(rand()%sub_width), (float)(rand()%sub_height), (float)N);
	} else {
		if ( n < N ) ++n;
		for (int i=n-1; i>0; --i)
			for (int j=0; j<T; ++j) {
				tocke[i][j]=tocke[i-1][j];
				tocke[i][j].z=tocke[i-1][j].z-1;
			}
		for (int i=0; i<T; ++i)
		{
			// svaku iducu krivulju dobivamo tako da prethodnu perturbiramo
			// i pri tome se pobrinemo da ne 'izletimo' iz ekrana
			tocke[0][i].x=glm::clamp(tocke[1][i].x+((float)(rand()%100-50)/4.0f), 0.0f, (float)sub_width);
			tocke[0][i].y=glm::clamp(tocke[1][i].y+((float)(rand()%100-50)/4.0f), 0.0f, (float)sub_height);
			tocke[0][i].z=(float)N;
		}
	}
	
	// svaku iducu boju dobivamo perturbacijom prethodne
	boja=boja+glm::vec3((float)(rand()%1000-500)/50000.0f, (float)(rand()%1000-500)/50000.0f, (float)(rand()%1000-500)/50000.0f );
	boja=glm::clamp(boja, 0.0f, 1.0f);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tocke), tocke, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute 0.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	); 
	
	glUseProgram(programID_t);
	glUniformMatrix4fv(MVMatrixID_t, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(PMatrixID_t, 1, GL_FALSE, &projection[0][0]);
	
	for (int i=0; i<n; ++i)
	{
		float v = ((float)(n-i))/n;
		bojan=v*boja;
		glUniform3fv(ColorID_t,1,&bojan[0]);
		glPatchParameteri(GL_PATCH_VERTICES,4);
		glDrawArrays(GL_PATCHES, 4*i, 4);
	}

	// onemogući slanje atributa nula shaderu
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
	
	stoperica.wait();
	
	glutPostRedisplay();
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

