// Kompajliranje:
// g++ -o geometry geometry.cpp util.cpp util_g.cpp -lGLEW -lGL -lGLU -lglut -lpthread

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
#include "util/Shader.h"


//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window; 
GLuint sub_width = 512, sub_height = 512;

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay		();
void myReshape		(int width, int height);
void myMouse		(int button, int state, int x, int y);
void myKeyboard		(unsigned char theKey, int mouseX, int mouseY);

Shader s,s2;
GLuint VAO;
GLuint programID, program_geometryID;
GLuint MVPMatrixID, MVMatrixID, NMatrixID, ColorID, MVPMatrix_geometryID, MVMatrix_geometryID, PMatrix_geometryID, Normallength_geometryID;

GLfloat normlen = 1.0;

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
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);

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
	// Stvori jedan VAO i njegov identifikator pohrani u VAO
	glGenVertexArrays(1, &VAO);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(VAO);

	// An array of 4 vectors which represents 4 vertices
	static const GLfloat g_vertex_buffer_data[] = {
	   -0.5f, -0.288675135f, -0.204124145f,
	   0.5f, -0.288675135f, -0.204124145f,
	   0.0f,  0.577350269f, -0.204124145f,
	   0.0f, 0.0f, 0.612372436f
	};

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

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
	
	// Stvaramo polje za 6 indeksa:
	static const ushort vindices[6] = { 0, 1, 2, 3, 0, 1};
  
	// This will identify our index buffer
	GLuint indexbuffer;
	// Generate 1 buffer, put the resulting identifier in indexbuffer
	glGenBuffers(1, &indexbuffer);
	// The following commands will talk about our 'indexbuffer' buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	// Give our indices to OpenGL.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ushort)*6, vindices, GL_STATIC_DRAW);

	std::cout << "Going to load programs... " << std::endl << std::flush;


    programID = s.load_shaders({"DEFAULT",
                                "SimpleFragmentShader.frag",
                                "SimpleGeometryShader.geom", "" , ""});

	
	if(programID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

    program_geometryID = s.load_shaders({"DEFAULT",
                                "SimpleFragmentShader.frag",
                                "GeometryShader.geom", "" , ""});
	

	if(program_geometryID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

	// Get a handle for our uniforms for later when drawing...
	MVPMatrixID = glGetUniformLocation(programID, "MVP");
	MVMatrixID = glGetUniformLocation(programID, "MV");
	NMatrixID = glGetUniformLocation(programID, "NORMAL");
	ColorID = glGetUniformLocation(programID, "outColor");
	MVPMatrix_geometryID = glGetUniformLocation(program_geometryID, "MVP");
	MVMatrix_geometryID = glGetUniformLocation(program_geometryID, "MV");
	Normallength_geometryID=glGetUniformLocation(program_geometryID, "normal_length");
	

	return true;
}

//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void myDisplay()
{
	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 	// Projection matrix : 30° Field of View, 1:1 ratio, display range : 0.1 unit <-> 100 units
 	glm::mat4 projection = glm::perspective(glm::radians(30.0f), (float)sub_width / (float)sub_height, 0.1f, 100.0f); 
  
  
 	// Camera matrix
 	glm::mat4 view = glm::lookAt(
	    glm::vec3(4,-3,-2), // Camera is at (4,-3,-2), in World Space
	    glm::vec3(0,0,0), // and looks at the origin
	    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
 	);
  
 	// Model matrix : an identity matrix (model will be at the origin)
 	glm::mat4 model = glm::mat4(1.0f);
 	// Our ModelViewProjection : multiplication of our 3 matrices
 	glm::mat4 mvp = projection * view * model; // Kasnije se mnozi matrica puta tocka - model matrica mora biti najbliza tocki
 	glm::mat4 mv = view * model; // ModelView matrica

	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator VAO
	glBindVertexArray(VAO);

	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);

	// Zatraži da shaderima upravlja naš program čiji je identifikator programID
	glUseProgram(programID);

	// Send our uniform variables to the currently bound shader program.
	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(MVMatrixID, 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix4fv(NMatrixID, 1, GL_FALSE, &model[0][0]);
	glm::vec3 color = glm::vec3(1.0,1.0,0.0); 
	glUniform3fv(ColorID,1,&color[0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//Crtanje tetraedra
	glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, (GLvoid*)0);
	
	// Zatraži da shaderima upravlja naš program čiji je identifikator program_geometryID
	glUseProgram(program_geometryID);
	
	// Send our uniform variables to the currently bound shader program.
	glUniformMatrix4fv(MVPMatrix_geometryID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(MVMatrix_geometryID, 1, GL_FALSE, &mv[0][0]);
	glUniform1f(Normallength_geometryID,normlen);
	
	//Crtanje normala teraedra
	glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, (GLvoid*)0);

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
    glViewport (0, 0, (GLsizei) width, (GLsizei) height);  //odredjuje da ce cijeli prozor biti predvidjen za crtanje
}

//*********************************************************************************
//	Mis.
//*********************************************************************************

void myMouse(int button, int state, int x, int y)
{
	//	Lijeva tipka - crta pocetnu tocku ili liniju.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		glutPostRedisplay();
	}
	//	Desna tipka - brise canvas. 
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		glutPostRedisplay();
	}
}


//*********************************************************************************
//	Tastatura tipke - r, g, b, k - mijenjaju boju.
//*********************************************************************************

void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
	switch (theKey)
	{
		case 'r':
			break;
	}
	glutPostRedisplay();
}

