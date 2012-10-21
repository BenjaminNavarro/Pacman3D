#include "OpenGLFunctions.h"

/***				Global variables			***/
GLMmodel*  	model;			        // glm model data structure
GLfloat    	scale;			        // original scale factor
GLuint		floorTex;				// handle for the floor texture
float 		camAngle = 90.0f;

//Draws the 3D scene
void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// translate the camera to follow pacman
	// Projection of pacman's z coordinate on the y and z axes depending on the camera angle
	glTranslatef(-PAC_Position.x, PAC_Position.z*sin(degToRad(camAngle)), -3.0f - PAC_Position.z*cos(degToRad(camAngle)));
	glRotatef(camAngle,1,0,0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, floorTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Draw the bottom base square
	glBegin(GL_QUADS);

	glTexCoord2f(0,0); glVertex3f( -GameBaseSize.x, 	GameBaseSize.y, 	GameBaseSize.z);
	glTexCoord2f(1,0); glVertex3f(	GameBaseSize.x, 	GameBaseSize.y, 	GameBaseSize.z);
	glTexCoord2f(1,1); glVertex3f(	GameBaseSize.x, 	GameBaseSize.y,    -GameBaseSize.z);
	glTexCoord2f(0,1); glVertex3f( -GameBaseSize.x, 	GameBaseSize.y,    -GameBaseSize.z);

	glEnd();

	glDisable(GL_TEXTURE_2D);

#if SHOW_GRID

	glBegin(GL_LINES);

	float winc = (2*GameBaseSize.x) / (float)N_CELLS_W;
	float hinc = (2*GameBaseSize.z) / (float)N_CELLS_H;

	for(int w=0; w<=N_CELLS_W; w++) {
		glVertex3f(-GameBaseSize.x + winc*w, 0.001,  GameBaseSize.z);
		glVertex3f(-GameBaseSize.x + winc*w, 0.001, -GameBaseSize.z);
	}

	for(int h=0; h<=N_CELLS_H; h++) {
		glVertex3f( GameBaseSize.x, 0.001, -GameBaseSize.z + hinc*h);
		glVertex3f(-GameBaseSize.x, 0.001, -GameBaseSize.z + hinc*h);
	}

	glEnd();

#endif

	glPushMatrix();

	glTranslatef(PAC_Position.x, PAC_RADIUS + PAC_Position.y, PAC_Position.z);
	glScalef(PAC_RADIUS,PAC_RADIUS,PAC_RADIUS);

	switch (PAC_Direction) {
	case NONE:
		// Nothing to do
		break;
	case FORWARD:
		setVect3(PAC_Angle, 0, 180, 0);
		glRotatef(180, 0, 1, 0);
		break;
	case BACKWARD:
		setVect3(PAC_Angle, 0, 0, 0);
		// Standard orientation
		break;
	case LEFT:
		setVect3(PAC_Angle, 0, -90, 0);
		glRotatef(-90, 0, 1, 0);
		break;
	case RIGHT:
		setVect3(PAC_Angle, 0, 90, 0);
		glRotatef(90, 0, 1, 0);
		break;
	}

	glmDraw(model, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);

	glPopMatrix();

	glutSwapBuffers();
}

// Loads the differents models used
void loadModels(void) {

	/* read in the pacman model */
	model = glmReadOBJ("models/pacman.obj");
	scale = glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0f);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	PAC_Position = gridToPos(locateOnGrid(PAC_Position));
}

//Called when a key is pressed
void handleKeypress(unsigned char key, int x, int y) {

	switch (key) {
	case 27: //Escape key
		glDeleteTextures(1,&floorTex);
		exit(0);
		break;
	case 'a':	// Stop (for testing purposes)
		PAC_Direction = NONE;
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
		PAC_Position.y = PAC_RADIUS*2;
		break;
	case 'p':	// Move low
		camAngle += 1;
		break;
	case 'm':	// Move low
		camAngle -= 1;
		break;
	}
}

//Called when a key is released
void handleKeyup(unsigned char key, int x, int y) {
	switch (key) {
	case 'h':	// Move high
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

	Image* floor = loadBMP("images/floor.bmp");
	floorTex = loadTexture(floor);

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

	gridPosition grid = locateOnGrid(PAC_Position);

	if((PAC_Direction == LEFT && grid.x > 0) || (PAC_Direction == RIGHT && grid.x < N_CELLS_W-1) || (PAC_Direction == FORWARD && grid.z > 0) || (PAC_Direction == BACKWARD && grid.z < N_CELLS_H-1)) {
		movePacman(PAC_Direction);
	}
	else {
		PAC_Direction = NONE;
		movePacman(PAC_Direction);
	}

	glutPostRedisplay();
	glutTimerFunc(speed, PAC_Update, 0);
}
