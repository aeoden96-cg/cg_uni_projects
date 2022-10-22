// Kompajliranje:
// g++ -o deRahmChaikin deRahmChaikin.cpp util_g.cpp util.cpp -lGLEW -lGL -lGLU -lglut -lpthread

#ifdef _WIN32
#include <windows.h>             //bit ce ukljuceno ako se koriste windows
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
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
GLuint sub_width = 512, sub_height = 512;
Shader s;
//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay		();
void myReshape		(int width, int height);
void myMouse		(int button, int state, int x, int y);
void myKeyboardFunc	(unsigned char theKey, int mouseX, int mouseY);
void dodaj_tocke();

GLuint vertexArrayID;
GLuint programID; 
GLuint MVPMatrixID, ColorID; 
GLuint vertexbuffer_s,vertexbuffer_n;

static const GLfloat color1[]={0.0f, 1.0f, 0.0f}, color2[]={0.5f, 0.5f, 0.0f}, color3[]={0.0f, 0.5f, 0.5f};

int WindowWidth, WindowHeight;
using namespace std;

int korak = 0, trenutno_tocaka = 0, novih_tocaka = 0, starih_tocaka = 0;
vector<glm::vec2> P;  //igra ulogu starih tocaka
vector<glm::vec2> nove_tocke;
vector<glm::vec2> sve_tocke;
vector<int> input_tocke;

void dodaj_tocke(){
	vector<glm::vec2> pomocni_vektor;
	glm::vec2 pomocna_tocka;
	P = nove_tocke;
	for( int i = 0; i < novih_tocaka; ++i )
			sve_tocke.push_back( nove_tocke[i] );	
	trenutno_tocaka = trenutno_tocaka + novih_tocaka;
	starih_tocaka = novih_tocaka;
	novih_tocaka = (novih_tocaka-1)*2;	
	for( int i = 0; i < starih_tocaka - 1; ++i ){
		pomocna_tocka = P[i] + 0.25f*( P[i+1] - P[i]);
		pomocni_vektor.push_back(pomocna_tocka);

		pomocna_tocka = P[i] + 0.75f*( P[i+1] - P[i]);
		pomocni_vektor.push_back(pomocna_tocka);		
		}
	nove_tocke = pomocni_vektor;
	
	}


void myKeyboardFunc (unsigned char key, int x, int y)
{
	if( key == 27 ){
		exit(0);

		}
		if(key == ' '){
		korak++;
		dodaj_tocke();
		glutPostRedisplay();
		}
	}

void mouse( int button, int state, int x, int y ) {
	glm::vec2 temp;
	
	if ( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) {
		float xPos = ((float)x)/((float)(WindowWidth-1));
		float yPos = ((float)y)/((float)(WindowHeight-1));

		yPos = 1.0f-yPos;			// Flip value since y position is from top row.
		
		temp.x = xPos;
		temp.y = yPos;
		nove_tocke.push_back(temp);
		
		novih_tocaka++;
		glutPostRedisplay();
		}
}

bool init_data(); // nasa funkcija za inicijalizaciju podataka

//*********************************************************************************
//	Glavni program.
//*********************************************************************************

int main(int argc, char ** argv)
{


	int ScreenWidth, ScreenHeight;
	
	// Sljedeci blok sluzi kao bugfix koji je opisan gore
	#ifdef LINUX_UBUNTU_SEGFAULT
        //ss2
        int i=pthread_getconcurrency();        
	#endif

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInit(&argc, argv);
	ScreenWidth = glutGet(GLUT_SCREEN_WIDTH);
	ScreenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	WindowWidth = ScreenWidth;
	WindowHeight = WindowWidth/3;
	glutInitWindowSize( WindowWidth, WindowHeight );
	glutInitWindowPosition( (ScreenWidth-WindowWidth) / 2, (ScreenHeight-WindowHeight) / 2 );

	window = glutCreateWindow("Glut OpenGL Prozor");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(mouse);
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


//*********************************************************************************
//	Funkcija u kojoj se radi inicijalizacija potrebnih VAO-a i VBO-ova. 
//*********************************************************************************

bool init_data()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glPointSize(8);
	glLineWidth(5);
	
	// Stvori jedan VAO i njegov identifikator pohrani u vertexArrayID
	glGenVertexArrays(1, &vertexArrayID);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(vertexArrayID);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer_s
	glGenBuffers(1, &vertexbuffer_s);
	// The following commands will talk about our 'vertexbuffer_s' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_s);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer_n
	glGenBuffers(1, &vertexbuffer_n);
	// The following commands will talk about our 'vertexbuffer_n' buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexbuffer_n);
	// Give our indices to OpenGL.
	
	std::cout << "Going to load programs... " << std::endl << std::flush;

	programID = s.load_shaders({"SimpleVertexShader.vert", "SimpleFragmentShader.frag","","",""});
	
	if(programID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

	// Get a handle for our uniforms for later when drawing...
	MVPMatrixID = glGetUniformLocation(programID, "MVP");
	ColorID = glGetUniformLocation(programID, "outColor");
	
	return true;
}

//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void myDisplay()
{
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 	// Projection matrix : 
 	glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f); 
  
 	// Model matrix : an identity matrix (model will be at the origin)
 	glm::mat4 model = glm::mat4(1.0f);
 	// Our ModelViewProjection : multiplication of our 2 matrices
 	glm::mat4 mvp = projection * model; // Kasnije se mnozi matrica puta tocka - model matrica mora biti najbliza tocki

	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator vertexArrayID
	glBindVertexArray(vertexArrayID);
	
	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);

	// Zatraži da shaderima upravlja naš program čiji je identifikator programID
	s.use();
	
	// Crtanje svih točaka
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_s);
	glBufferData(GL_ARRAY_BUFFER, ((int) sve_tocke.size())*2*sizeof(float), &sve_tocke[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(
	   0,                  // attribute 0.
	   2,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniform3fv(ColorID,1,&color1[0]);
	glDrawArrays(GL_POINTS, 0, trenutno_tocaka);
	
	// Crtanje novog kontrolnog poligona
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_n);
	glBufferData(GL_ARRAY_BUFFER, ((int) nove_tocke.size())*2*sizeof(float), &nove_tocke[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
	   0,                  // attribute 0.
	   2,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
		
	if( korak % 2 )
		glUniform3fv(ColorID,1,&color2[0]);
	else
		glUniform3fv(ColorID,1,&color3[0]);
		
	cout<<novih_tocaka<<endl;
	glDrawArrays(GL_POINTS, 0, novih_tocaka);
	glDrawArrays(GL_LINE_STRIP, 0, novih_tocaka);

	// onemogući slanje atributa nula shaderu
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void myReshape(int width, int height)
{
	sub_width = (width>1) ? width : 2;                  // zapamti novu sirinu prozora
    sub_height = (height>1) ? height : 2;				// zapamti novu visinu prozora
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);	
}

