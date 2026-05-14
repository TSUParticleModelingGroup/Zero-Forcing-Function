// nvcc zeroForcing.cu -o zeroForcing -lglut -lm -lGLU -lGL
//To force kill hit "control c" in the window you launched it from.
// -lGL -lm -lX11 -lXrandr -lXi -lXxf86vm -lpthread -ldl

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
//#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
using namespace std;

// defines
#define BOLD_ON  "\e[1m"
#define BOLD_OFF   "\e[m"
#define PI 3.141592654
#define LINKS_PER_NODE 20
#define X_WINDOW_SIZE 1000
#define Y_WINDOW_SIZE 1000
#define BIG_INT 1000000000
#define BIG_FLOAT 100000000.0

FILE* MovieFile;
int* Buffer;
char NodesEdgesFileName[256];

//int MovieFlag; // 
int ViewFlag; // 0 orthoganal, 1 fulstum
int MovieFlag; // 0 movie off, 1 movie on
bool SelectionFlag; // 0 selction off, 1 selection on
bool RotateTranslateFlag; // 0 rotate, 1 translate
int IterateFlag; // Runs one iteration
int FinishFlag; // Finishes the job

float LineWidth;
float NodeRadius;
float SelectionSize;
float ScrollSpeed;
float ZoomSpeed;
float RotateSpeed;
double MouseX, MouseY, MouseZ;
float4 CenterOfSimulation;
float4 AngleOfSimulation;

int NumberOfNodes;
int NumberOfEdges;
int LinksPerNode = LINKS_PER_NODE;

struct edgeAtributesStructure
{
	int nodeA;
	int nodeB;    
};
edgeAtributesStructure *Edge;

struct nodeAtributesStructure
{
	float4 position;
	int on;
	int update;
	int connection[LINKS_PER_NODE];
	float4 color;
};
nodeAtributesStructure *Node;

// Window globals
static int Window;
int XWindowSize;
int YWindowSize; 
double Near;
double Far;
double EyeX;
double EyeY;
double EyeZ;
double CenterX;
double CenterY;
double CenterZ;
double UpX;
double UpY;
double UpZ;

// Prototyping functions
// zeroForcing
void setup();
int upDate();
void upGrade();
void zeroForcing();
void terminalPrint();
int main(int, char**);
// setNodesAnsEdges
void readSimulationParameters();
void ReadInAndSetNodes();
void ReadInAndSetEdges();
void linkNodes();
// drawAndCallBackFunctions
void drawPicture();
void Display(void);
void reshape(int w, int h);
void orthoganialView();
void fulstrumView();
void KeyPressed(unsigned char key, int x, int y);
void mymouse(int button, int state, int x, int y);

#include "./setNodesAndEdges.h"
#include "./drawAndCallBackFunctions.h"

void setup()
{	
	XWindowSize = X_WINDOW_SIZE;
	YWindowSize = Y_WINDOW_SIZE; 
	
	readSimulationParameters();
	ReadInAndSetNodes();
	ReadInAndSetEdges();
	linkNodes();
	
	AngleOfSimulation.x = 0.0;
	AngleOfSimulation.y = 1.0;
	AngleOfSimulation.z = 0.0;
	
	//MovieOn = 0;
	IterateFlag = 0;
	FinishFlag = 0;
	SelectionFlag = 0;
	RotateTranslateFlag = 0;
	SelectionSize = 0.05;
	ScrollSpeed = 0.01;
	ZoomSpeed = 0.01;
	RotateSpeed = 0.01;
	
	//MovieFlag = 0;
	
	MouseX = 0.0;
	MouseY = 0.0;
	float maxZ = -BIG_FLOAT;
	for(int i = 0; i < NumberOfNodes; i++)
	{
		if(maxZ < Node[i].position.z) maxZ = Node[i].position.z;
	}
	MouseZ = maxZ;
	
	double averageSeperation = 0.0;
	float dx,dy,dz;
	for(int i = 0; i < NumberOfEdges; i++)
	{
		dx = Node[Edge[i].nodeA].position.x - Node[Edge[i].nodeB].position.x;
		dy = Node[Edge[i].nodeA].position.y - Node[Edge[i].nodeB].position.y;
		dz = Node[Edge[i].nodeA].position.z - Node[Edge[i].nodeB].position.z;
		averageSeperation += sqrt(dx*dx + dy*dy + dz*dz);
	}
	averageSeperation /= (double)NumberOfEdges;
	NodeRadius = 0.3*averageSeperation;
	LineWidth = 0.02*averageSeperation;
	
	terminalPrint();
}

int upDate()
{
	int updateCount = 0;
	for(int i = 0; i < NumberOfNodes; i++)
	{
		int count = 0;
		for(int j = 0; j < LinksPerNode; j++)
		{
			if(Node[Node[i].connection[j]].on == 1) count++;
		}
		if(1 < count) 
		{
			Node[i].update = 1;
			updateCount++;
		}
	}
	return(updateCount);
}

void upGrade()
{
	for(int i = 0; i < NumberOfNodes; i++)
	{
		if(Node[i].update == 1) 
		{
			Node[i].on = 1;
			Node[i].color.x = 0.0;
			Node[i].color.y = 0.0;
			Node[i].color.z = 1.0;
			
			Node[i].update = 0;
		}
	}
}

void zeroForcing()
{	
	int updateCount = BIG_INT;
	if(IterateFlag == 1 || FinishFlag == 1)
	{	
		updateCount = upDate();
		upGrade();
		drawPicture();
		IterateFlag = 0;
	}
	if(updateCount == 0)
	{
		printf("\n\n The run has finished.\n\n");
	}
}

void terminalPrint()
{
	system("clear");
	//printf("\033[0;34m"); // blue.
	//printf("\033[0;36m"); // cyan
	//printf("\033[0;33m"); // yellow
	//printf("\033[0;31m"); // red
	//printf("\033[0;32m"); // green
	printf("\033[0m"); // back to white.
	
	printf("\n");
	printf("\033[0;33m");
	printf("\n **************************** Terminal Comands ****************************");
	printf("\033[0m");
	printf("\n q: Quits the run");
	printf("\n");
	printf("\n 1: To run one iteration");
	printf("\n f: A toggle to finish the current setup");
	if(FinishFlag == 1) printf("\n \033[0;32m You are in finish mode.");
	else              printf("\n \033[0;32m You are in not in finish mode.");
	printf("\033[0m");
	printf("\n");
	printf("\n S: Screenshot");
	printf("\n M: Movie on off toggle");
	if(MovieFlag == 1) printf("\n \033[0;32m You are in movie mode.");
	else               printf("\n \033[0;32m You are in not in movie mode.");
	printf("\033[0m");
	printf("\n");
	printf("\n s: A toggle to turn on and off selection sphere");
	if(SelectionFlag == 1) printf("\n \033[0;32m You are in selection mode.");
	else                  printf("\n \033[0;32m You are in not in selection mode.");
	printf("\033[0m");
	printf("\n Left mouse click to sellect a node");
	printf("\n Right mouse click to unsellect a node");
	printf("\n =/-: Increase and decrease slection shpere radius");
	printf("\n The shpere will follow the mouse left and right.");
	printf("\n The middle wheel will move the sphere in and out.");
	printf("\n");
	printf("\n t: A toggle to switch between translation or rotation of the view");
	if(RotateTranslateFlag == 1) printf("\n \033[0;32m You are in rotation mode.");
	else                         printf("\n \033[0;32m You are in translation mode.");
	printf("\033[0m");
	printf("\n x/X: Translate/Rotate x-axis");
	printf("\n y/Y: Translate/Rotate y-axis");
	printf("\n z/Z: Translate/Rotate z-axis");
	printf("\n");
	printf("\n v: A toggle to go between fulstrum and orthoganal views.");
	if(ViewFlag == 0) printf("\n \033[0;32m You are in fulstrum view.");
	else              printf("\n \033[0;32m You are in orthoganal view.");
	printf("\033[0;33m");
	printf("\n ********************************************************************");
	printf("\033[0m");
	printf("\n");
}

int main(int argc, char** argv)
{
	setup();

	// Clip plains
	Near = 0.1;
	Far = 80.0;

	//Direction here your eye is located location
	EyeX = 0.0;
	EyeY = 0.0;
	EyeZ = 0.5;

	//Where you are looking
	CenterX = 0.0;
	CenterY = 0.0;
	CenterZ = 0.0;

	//Up vector for viewing
	UpX = 0.0;
	UpY = 1.0;
	UpZ = 0.0;
	
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(XWindowSize,YWindowSize);
	glutInitWindowPosition(5,5);
	Window = glutCreateWindow("SVT");
	
	gluLookAt(EyeX, EyeY, EyeZ, CenterX, CenterY, CenterZ, UpX, UpY, UpZ);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.2, 0.2, -0.2, 0.2, Near, Far);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	//GLfloat light_position[] = {(float)EyeX, (float)EyeY, (float)EyeZ, 0.0};
	//GLfloat light_position[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
	GLfloat light_ambient[]  = {0.0, 0.0, 0.0, 1.0};
	GLfloat light_diffuse[]  = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat mat_specular[]   = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_shininess[]  = {10.0};
	glShadeModel(GL_SMOOTH);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	
	//glutMouseFunc(mouseWheelCallback);
	//glutMouseWheelFunc(mouseWheelCallback);
	//glutMotionFunc(mouseMotionCallback);
    	glutPassiveMotionFunc(mousePassiveMotionCallback);
	glutDisplayFunc(Display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mymouse);
	glutKeyboardFunc(KeyPressed);
	glutIdleFunc(idle);
	glutSetCursor(GLUT_CURSOR_DESTROY);
	glEnable(GL_DEPTH_TEST);
	
	glutMainLoop();
	return 0;
}
