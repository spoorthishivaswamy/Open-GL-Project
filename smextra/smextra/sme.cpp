#include <math.h>
#include<stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include<Windows.h>

#define MAX_PARTICLES 1000

float angle = 0.0, deltaAngle = 0.0, ratio;
float x = 0.0f, y = 1.75f, z = 5.0f;
float lx = 0.0f, ly = 0.0f, lz = -1.0f;
int deltaMove = 0, h, w;
int font = (int)GLUT_BITMAP_8_BY_13;
static GLint snowman_display_list;
int bitmapHeight = 13;
float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
float pan = 0.0;
float tilt = 0.0;
int frame, time, timebase = 0;

int loop;


typedef struct {
	// Life
	bool alive; // is the particle alive?
	float life; // particle lifespan
	float fade; // decay
				// color
	float red;
	float green;
	float blue;
	// Position/direction
	float xpos;
	float ypos;
	float zpos;
	// Velocity/Direction, only goes down in y dir
	float vel;
	// Gravity
	float gravity;
}particles;

// Paticle System
particles par_sys[MAX_PARTICLES];

long int par = MAX_PARTICLES;
char s[30];

void initWindow();
void initParticles(int i) {
	par_sys[i].alive = true;
	par_sys[i].life = 1.0;
	par_sys[i].fade = float(rand() % 50) / 1000.0f + 0.003f;

	par_sys[i].xpos = (float)(rand() % 100) - 50;
	par_sys[i].ypos = 25;
	par_sys[i].zpos = (float)(rand() % 100) - 10;

	par_sys[i].red = 0.5;
	par_sys[i].green = 0.5;
	par_sys[i].blue = 1.0;

	par_sys[i].vel = velocity;
	par_sys[i].gravity = -0.8;//-0.8;

}

void changeSize(int w1, int h1)
{

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h1 == 0)
		h1 = 1;

	w = w1;
	h = h1;
	ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45, ratio, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z,
		x + lx, y + ly, z + lz,
		0.0f, 1.0f, 0.0f);


}




void drawSnowMan() {


	glColor3f(1.0f, 1.0f, 1.0f);

	// Draw Body
	glTranslatef(0.0f, 0.75f, 0.0f);
	glutSolidSphere(0.75f, 20, 20);


	// Draw Head
	glTranslatef(0.0f, 1.0f, 0.0f);
	glutSolidSphere(0.25f, 20, 20);

	// Draw Eyes
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.05f, 0.10f, 0.18f);
	glutSolidSphere(0.05f, 10, 10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f, 10, 10);
	glPopMatrix();

	// Draw Nose
	glColor3f(1.0f, 0.5f, 0.5f);
	glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.08f, 0.5f, 10, 2);
}



GLuint createDL() {
	GLuint snowManDL;

	// Create the id for the list
	snowManDL = glGenLists(2);

	glNewList(snowManDL + 1, GL_COMPILE);
	drawSnowMan();
	glEndList();
	// start list
	glNewList(snowManDL, GL_COMPILE);

	// call the function that contains the rendering commands
	for (int i = -5; i < 5; i++)
		for (int j = -5; j < 5; j++) {
			glPushMatrix();
			glTranslatef(i*10.0, 0, j * 10.0);
			glCallList(snowManDL + 1);
			glPopMatrix();
		}

	// endList
	glEndList();

	return(snowManDL);
}

void initScene() {

	glEnable(GL_DEPTH_TEST);
	snowman_display_list = createDL();

}

void orientMe(float ang) {


	lx = sin(ang);
	lz = -cos(ang);
	glLoadIdentity();
	gluLookAt(x, y, z,
		x + lx, y + ly, z + lz,
		0.0f, 1.0f, 0.0f);
}


void moveMeFlat(int i) {
	x = x + i*(lx)*0.1;
	z = z + i*(lz)*0.1;
	glLoadIdentity();
	gluLookAt(x, y, z,
		x + lx, y + ly, z + lz,
		0.0f, 1.0f, 0.0f);
}

void setOrthographicProjection() {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	gluOrtho2D(0, w, 0, h);
	// invert the y axis, down is positive
	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(0, -h, 0);
	glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection() {
	// set the current matrix to GL_PROJECTION
	glMatrixMode(GL_PROJECTION);
	// restore previous settings
	glPopMatrix();
	// get back to GL_MODELVIEW matrix
	glMatrixMode(GL_MODELVIEW);
}

void renderBitmapString(float x, float y, void *font, char *string)
{

	char *c;
	// set position to start drawing fonts
	glRasterPos2f(x, y);
	// loop all the characters in the string
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void drawSnow() {
	float x, y, z;
	for (loop = 0; loop < par; loop = loop + 5) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(1.0, 1.0, 1.0);
			glPushMatrix();
			glTranslatef(x, y, z);
			glutSolidSphere(0.2, 16, 16);
			glPopMatrix();

			// Update values
			//Move
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			if (par_sys[loop].ypos <= -10) {

				par_sys[loop].life = -1.0;
			}

			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}


void renderScene(void) {

	if (deltaMove)
		moveMeFlat(deltaMove);
	if (deltaAngle) {
		angle += deltaAngle;
		orientMe(angle);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw ground

	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();
	glCallList(snowman_display_list);

	//drawSnow();
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf(s, "FPS:%4.2f", frame*1000.0 / (time - timebase));
		timebase = time;
		frame = 0;
	}

	glColor3f(0.0f, 1.0f, 1.0f);
	setOrthographicProjection();
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(30, 15, (void *)font, "Snow Man");
	renderBitmapString(30, 55, (void *)font, "Esc - Quit");
	renderBitmapString(30, 35, (void *)font, s);
	glPopMatrix();
	resetPerspectiveProjection();

	glutSwapBuffers();
}







void renderScene1(void) {

	if (deltaMove)
		moveMeFlat(deltaMove);
	if (deltaAngle) {
		angle += deltaAngle;
		orientMe(angle);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw ground

	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();

	// Draw 36 Snow Men

	glCallList(snowman_display_list);

	drawSnow();
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf(s, "FPS:%4.2f", frame*1000.0 / (time - timebase));
		timebase = time;
		frame = 0;
	}

	glColor3f(0.0f, 1.0f, 1.0f);
	setOrthographicProjection();
	glPushMatrix();
	glLoadIdentity();

	renderBitmapString(30, 15, (void *)font, "Snow Man");
	renderBitmapString(30, 55, (void *)font, "Esc - Quit");
	renderBitmapString(30, 35, (void *)font, s);
	glPopMatrix();
	resetPerspectiveProjection();

	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
		exit(0);
}

void pressKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT: deltaAngle = -0.01f; break;
	case GLUT_KEY_RIGHT: deltaAngle = 0.01f; break;
	case GLUT_KEY_UP: deltaMove = 1; break;
	case GLUT_KEY_DOWN: deltaMove = -1; break;
	}

}

void releaseKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT: if (deltaAngle < 0.0f)
		deltaAngle = 0.0f;
		break;
	case GLUT_KEY_RIGHT: if (deltaAngle > 0.0f)
		deltaAngle = 0.0f;
		break;
	case GLUT_KEY_UP:  if (deltaMove > 0)
		deltaMove = 0;
		break;
	case GLUT_KEY_DOWN: if (deltaMove < 0)
		deltaMove = 0;
		break;
	}
}


void initWindow() {
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	for (loop = 0; loop < MAX_PARTICLES; loop++) {
		initParticles(loop);
	}
	initScene();

}




void initWindow1() {
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
	glutDisplayFunc(renderScene1);
	glutIdleFunc(renderScene1);
	glutReshapeFunc(changeSize);
	for (loop = 0; loop < par; loop++) {
		initParticles(loop);
	}
	initScene();

}



void initWindow0() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 0, 0);
	//glEnable(GL_DEPTH_TEST);

	char string[64];
	sprintf(string, "something");
	//printtext(10, 10, string);

	glutSwapBuffers();

}


void menu(int id) {
	if (id == 0)
	{
		exit(0);
	}
	if (id == 1)
	{
		printf("Snow Fall Started with number of particles = %d\n", MAX_PARTICLES);
		PlaySound(TEXT("WhatsApp_Audio_2017-05-29_at_22_20_28.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
		initWindow1();
	}
	if (id == 2)
	{
		par = MAX_PARTICLES;
		printf("Snow Fall Stopped And Particle Value is Reset\n");
		PlaySound(NULL, NULL, 0);
		initWindow();
	}
	if (id == 3)
	{
		par += 1000;
		printf("Snow Particles Icreased and Particle Value is = %d\n", par);
		initWindow1();

	}
	if (id == 4)
	{
		par -= 1000;
		printf("Snow Particles Decreased and Particle Value is = %d\n", par);
		initWindow1();
	}
}
/*void title()												// to draw the starting screen
{
	//printf("hey\n");
	renderBitmapString(30, 15, (void *)font, "Computer Graphics Project : SnowMan Simulation Using OpenGL");
	renderBitmapString(30, 35, (void *)font, "Submitted by:");
	renderBitmapString(30, 55, (void *)font, "R SAIPRASANTH\t USN : 1RN14CS075");
	renderBitmapString(30, 75, (void *)font, "S MEGHA \tUSN:1RN14CS085");
	renderBitmapString(30, 95, (void *)font, "SPOORTHI S \tUSN:1RN14CS105");
	renderBitmapString(30, 115, (void *)font, "RNS Institute Of Techinology");
	glFlush();
}

void delay()
{

	int k = 10;
	while (k != 0)
	{
		k--;
		int i =50;
		while (i != 0)
		{
			i--;

		}
	}

}*/
void display()
{
	gluOrtho2D(-100, 200, -100, 200);
	glClearColor(1, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0, 1, 1);
	//title();
	//delay();
}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 360);
	glutCreateWindow("Christmas time");
	glutDisplayFunc(display);
	// register all callbacks
	//title();
	initWindow();
	glutCreateMenu(menu);
	glutAddMenuEntry("Start Snowfall", 1);
	glutAddMenuEntry("Stop Snowfall", 2);
	glutAddMenuEntry("Increase number of snow particles ", 3);
	glutAddMenuEntry("Decrease number of snow particles ", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	return(0);
}
