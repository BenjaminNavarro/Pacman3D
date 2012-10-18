#include "OpenGLFunctions.h"

/***				Global variables			***/
GLMmodel*  	model;			        /* glm model data structure */
GLfloat    	scale;			        /* original scale factor */

float 		camAngle = 45.0f;

void loadModels(void) {

    /* read in the model */
    model = glmReadOBJ("models/pacman.obj");
    scale = glmUnitize(model);
    glmFacetNormals(model);
    glmVertexNormals(model, 90.0f);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
}

//Draws the 3D scene
void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -5.5f);
	glRotatef(camAngle,1,0,0);

	glPushMatrix();
		glColorRGB(0xAA, 0xAA, 0xAA);
	glPopMatrix();

	// Draw the bottom base square
	glBegin(GL_QUADS);

		point base={GAME_BASE_SIZE,0,GAME_BASE_SIZE};

	    glVertex3f(-base.x, base.y, base.z);
	    glVertex3f(base.x, base.y, base.z);
	    glVertex3f(base.x, base.y, -base.z);
	    glVertex3f(-base.x, base.y, -base.z);

	glEnd();

    glPushMatrix();

		glTranslatef(PAC_Position.x, PAC_RADIUS + PAC_Position.y, PAC_Position.z);
    	glScalef(PAC_RADIUS,PAC_RADIUS,PAC_RADIUS);

    	glmDraw(model, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);

	glPopMatrix();

	glutSwapBuffers();
}

//Called when a key is pressed
void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			exit(0);
			break;
		case 'z':	// Move forward
			PAC_Direction = FORWARD;
			break;
		case 's':	// Move backward
			PAC_Direction = BACKWARD;
			break;
		case 'q':	// Move left
			PAC_Direction = LEFT;
			break;
		case 'd':	// Move right
			PAC_Direction = RIGHT;
			break;
		case 'h':	// Move high
			PAC_Position.y = 1;
			break;
		case 'l':	// Move low
			PAC_Position.y = -1;
			break;
	}
}

//Called when a key is released
void handleKeyup(unsigned char key, int x, int y) {
	switch (key) {
		case 'h':	// Move high
			PAC_Position.y = 0;
			break;
		case 'l':	// Move low
			PAC_Position.y = 0;
			break;
	}
}

//Initializes 3D rendering
void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); //Enable alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blend function
	glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	loadModels();
}

//Called when the window is resized
void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

void PAC_Update(int value) {

	switch (PAC_Direction) {
	case NONE:
		// Nothing to do
		break;
	case FORWARD:
		if(abs((PAC_Position.z - PAC_RADIUS - PAC_PosInc)) < GAME_BASE_SIZE) {
			PAC_Position.z -= PAC_PosInc;
		}
		break;
	case BACKWARD:
		if(abs((PAC_Position.z + PAC_RADIUS + PAC_PosInc)) < GAME_BASE_SIZE) {
			PAC_Position.z += PAC_PosInc;
		}
		break;
	case LEFT:
		if(abs((PAC_Position.x - PAC_RADIUS - PAC_PosInc)) < GAME_BASE_SIZE) {
			PAC_Position.x -= PAC_PosInc;
		}
		break;
	case RIGHT:
		if(abs((PAC_Position.x + PAC_RADIUS + PAC_PosInc)) < GAME_BASE_SIZE) {
			PAC_Position.x += PAC_PosInc;
		}
		break;
	}
	
	glutPostRedisplay();
	glutTimerFunc(speed, PAC_Update, 0);
}
