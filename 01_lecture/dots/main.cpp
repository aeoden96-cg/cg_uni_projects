// Kompajliranje:
// g++ -o dots dots.cpp util.cpp -lGLEW -lGL -lGLU -lglut -lpthread

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
//#include "util/util.hpp"
#include "util/Shader.h"



//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window; 
GLuint sub_width = 500, sub_height = 500; //sub_width = 256, sub_height = 256;

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay		();
void myReshape		(int width, int height);
void myMouseFunc	(int button, int state, int x, int y);
void myKeyboardFunc	(unsigned char theKey, int mouseX, int mouseY);
bool init_data();
void removeFirstPoint();
void removeLastPoint();
void addNewPoint( float x, float y );
void rotatefunkcija();

GLuint VAO;
GLuint programID;
GLuint MVPMatrixID;
GLuint ColorID;


//std::vector<std::string> list = {"SimpleVertexShader.vert","frag.frag"};

#define MaxNumPts 64
GLfloat PointArray [MaxNumPts][2];
int NumPts = 0;
static const GLfloat color1[]={1.0f, 0.0f, 0.8f, 1.0f}, color2[]={0.0f, 0.0f, 0.0f, 1.0f};
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 projection;

int WindowHeight;
int WindowWidth;
Shader s;
void rotatefunkcija()
{model = glm::rotate (model,0.17f,glm::vec3(0.0f,0.0f,1.0f)); // radijani!
}

void myKeyboardFunc (unsigned char key, int x, int y)
{
	switch (key) {
	case 'f':
		removeFirstPoint();
		glutPostRedisplay();
		break;
	case 'l':
		removeLastPoint();
		glutPostRedisplay();
		break;
	case 27:			// Escape key
		exit(0);
		break;
	case 'r':
		rotatefunkcija();
		glutPostRedisplay();
		break;
	}
}


void removeFirstPoint() {
	int i; 
	if ( NumPts>0 ) {
		// Makni prvu tocku, i pomakni slijedece
		NumPts--;
		for ( i=0; i<NumPts; i++ ) {
			PointArray[i][0] = PointArray[i+1][0];
			PointArray[i][1] = PointArray[i+1][1]; 
		}
	}
}

// Lijevi klik za kontrolnu tocku
void myMouseFunc( int button, int state, int x, int y ) {
	if ( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) {
		float xPos = ((float)x)/((float)(WindowWidth-1));
		float yPos = ((float)y)/((float)(WindowHeight-1));

		yPos = 1.0f-yPos; // u koordinatnom sustavu prozora, y-koordinate pixela povećavaju se od gore prema dolje

		addNewPoint( xPos, yPos );
		glutPostRedisplay();
	}
}

// Makni zadnju tocku
void removeLastPoint() {
	if ( NumPts>0 ) {
		NumPts--;
	}
}

// Nova tocka na kraj liste
// Makni prvu ako ih je previse
void addNewPoint( float x, float y ) {
	if ( NumPts>=MaxNumPts ) {
		removeFirstPoint();
	}
	PointArray[NumPts][0] = x;
	PointArray[NumPts][1] = y; 
	NumPts++;
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
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB ); 
	glutInitWindowSize(sub_width,sub_height);
	glutInitWindowPosition(100,100);
	glutInit(&argc, argv);

	window = glutCreateWindow("Glut OpenGL Prozor");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouseFunc);
	glutKeyboardFunc(myKeyboardFunc);

	glewExperimental = GL_TRUE;
	//glewInit();

    GLenum err = glewInit();
    if (err != GLEW_OK)
        exit(1); // or handle the error in a nicer way
    if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
        exit(1); // or handle the error in a nicer way

	if(!init_data()) return 1;

	glutMainLoop();
    	return 0;
}

//*********************************************************************************
//	Funkcija u kojoj se radi inicijalizacija potrebnih VAO-a i VBO-ova. 
//*********************************************************************************

bool init_data()
{
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

	// Velike tocke i siroke linije
	glPointSize(8);
	glLineWidth(5);

	// Okrugle tocke i antialijasiranje, ovisno o implementaciji
	
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);	// Okrugle a ne cetvrtaste tocke
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Antialijasiranje 
	
	// Stvori jedan VAO i njegov identifikator pohrani u VAO
	glGenVertexArrays(1, &VAO);
	// Učini taj VAO "trenutnim". Svi pozivi glBindBuffer(...) ispod upisuju veze u trenutni (dakle ovaj) VAO.
	glBindVertexArray(VAO);

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	// Podesi da se 0. atribut dohvaca iz vertex spremnika
	glVertexAttribPointer(
	   0,                  // attribute 0.
	   2,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	); 


	std::cout << "Going to load programs... " << std::endl << std::flush;

    programID = s.load_shaders({"SimpleVertexShader.vert","SimpleFragmentShader.frag" , "" , "" , ""});
    //	programID = loadShaders("SimpleVertexShader.vert", "frag.frag");


	if(programID==0) {
		std::cout << "Zbog grešaka napuštam izvođenje programa." << std::endl;
		return false;
	}

	// Get a handle for our "MVP" and "clr" uniform for later when drawing...
	MVPMatrixID = glGetUniformLocation(programID, "MVP");
	ColorID = glGetUniformLocation(programID, "clr");

	return true;
}

//*********************************************************************************
//	Osvjezavanje prikaza. (nakon preklapanja prozora) 
//*********************************************************************************

void myDisplay()
{
	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
 	glClear(GL_COLOR_BUFFER_BIT);

 	glBufferData(GL_ARRAY_BUFFER, sizeof(PointArray), PointArray, GL_DYNAMIC_DRAW);
 	
 	// Our ModelViewProjection : multiplication of our 2 matrices
 	glm::mat4 mvp = projection * model; // Kasnije se mnozi matrica puta tocka - model matrica mora biti najbliza tocki
	
	// Postavi da se kao izvor toka vertexa koristi VAO čiji je identifikator VAO
	glBindVertexArray(VAO);

	// omogući slanje atributa nula shaderu - pod indeks 0 u init smo povezali pozicije vrhova (x,y,z)
	glEnableVertexAttribArray(0);

	// Zatraži da shaderima upravlja naš program čiji je identifikator programID
//	glUseProgram(programID);
    s.use();

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);

	// Zatraži crtanje linija i točaka
	glUniform4fv(ColorID,1,color1); 
	glDrawArrays(GL_LINE_STRIP, 0, NumPts); 
	glUniform4fv(ColorID,1,color2);
	glDrawArrays(GL_POINTS, 0, NumPts);

	// onemogući slanje atributa nula shaderu
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}

//*********************************************************************************
//	Promjena velicine prozora.
//*********************************************************************************

void myReshape(int w, int h)
{
	WindowHeight = (h>1) ? h : 2;
	WindowWidth = (w>1) ? w : 2;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	projection = glm::ortho(0.0f,1.0f,0.0f,1.0f,-1.0f,1.0f); // In world coordinates
}


