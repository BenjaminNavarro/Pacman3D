#include "OpenGLFunctions.h"

/***				Global variables			***/
GLMmodel*  		PacmanModel;						// glm model data structure for pacman
GLMmodel*		PacmanAnimation[PAC_ANIM_FRAMES];	// array of the different pacman models
GLMmodel*		Ghost[GHOST_COUNT];					// array of the different ghosts models
GLuint			floorTex;							// handle for the floor texture
float 			camAngle = 45.0f;
direction 		newDirection = NONE;



//Draws the 3D scene
void drawScene() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// translate the camera to follow pacman
	// Projection of pacman's z coordinate on the y and z axes depending on the camera angle
	glTranslatef(-PAC_Position.x, PAC_Position.z*sin(degToRad(camAngle)), -2.0f - PAC_Position.z*cos(degToRad(camAngle)));
	glRotatef(camAngle,1,0,0);

	// Enable the use of 2D textures
	glEnable(GL_TEXTURE_2D);

		// Load the floor texture
		glBindTexture(GL_TEXTURE_2D, floorTex);

		// Set the texture rendering to linear
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

	// Draw the content of the game board : coins, big coins, fruits
	renderGame();

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

		switch (newDirection) {
		case NONE:
			// Nothing to do
			break;
		case FORWARD:
			glRotatef(180, 0, 1, 0);
			break;
		case BACKWARD:
			// Standard orientation
			break;
		case LEFT:
			glRotatef(-90, 0, 1, 0);
			break;
		case RIGHT:
			glRotatef(90, 0, 1, 0);
			break;
		}

		glmDraw(PacmanModel, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);

	glPopMatrix();

	for(int i=0; i<GHOST_COUNT; i++) {

		glPushMatrix();


		glTranslatef(Ghost_Position[i].x, GHOST_SCALE + Ghost_Position[i].y, Ghost_Position[i].z);
		glScalef(GHOST_SCALE,GHOST_SCALE,GHOST_SCALE);

		switch (Ghost_Direction[i]) {
		case NONE:
			// Nothing to do
			break;
		case FORWARD:
			glRotatef(180, 0, 1, 0);
			break;
		case BACKWARD:
			glRotatef(0, 0, 1, 0);
			break;
		case LEFT:
			glRotatef(-90, 0, 1, 0);
			break;
		case RIGHT:
			glRotatef(90, 0, 1, 0);
			break;
		}

		glmDraw(Ghost[i], GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);

		glPopMatrix();

	}


	// Display the HUD on the screen
	displayHUD();

	// Check if Pacman is touching a ghost
	checkGhosts();

	glutSwapBuffers();
}

// Loads the differents models used
void loadModels(void) {

	char file[30];

	for(int i=0; i<PAC_ANIM_FRAMES; i++) {
		sprintf(file, "models/animation/pacman_%d.obj",i);			// Set the path to the frame model
		PacmanAnimation[i] = glmReadOBJ(file);						// Read it
		glmUnitize(PacmanAnimation[i]);								// Put it the (0,0,0) with a size of 1
		glmFacetNormals(PacmanAnimation[i]);						// Generates the facet normals
		glmVertexNormals(PacmanAnimation[i], 90.0f);				// Smooth the model up to 90¡
	}

	for(int i=0; i<GHOST_COUNT; i++) {
		sprintf(file, "models/ghost_%d.obj",i);						// Set the path to the frame model
		Ghost[i] = glmReadOBJ(file);								// Read it
		glmUnitize(Ghost[i]);										// Put it the (0,0,0) with a size of 1
		glmFacetNormals(Ghost[i]);									// Generates the facet normals
		glmVertexNormals(Ghost[i], 90.0f);							// Smooth the model up to 90¡

		sendGhostHome(i);											// Move the ghost the its origin position
	}

	// Set PacmanModel pointing to the first frame
	PacmanModel = PacmanAnimation[0];

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	sendPacmanHome();

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

	gameBoardInit();

	Image* floor = loadBMP("images/floor.bmp");
	floorTex = loadTexture(floor);

	loadModels();

	t3dInit();

	srand(time(NULL));

}

//Called when the window is resized, forces the game dimmension to SCREEN_WIDTH and SCREEN_HEIGHT
void handleResize(int w, int h) {
	glViewport((w - SCREEN_WIDTH)/2, (h - SCREEN_HEIGHT)/2, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT, 1.0, 200.0);
}

void PAC_Update(int value) {
	static gridPosition	nextPosition;

	// Locate Pacman on the grid
	if(PAC_Direction != NONE)
		nextPosition = getNextPosition();
	else
		nextPosition = locateOnGrid(PAC_Position);

	switch (PAC_Direction) {
		case NONE:
			newDirection = NONE;
			break;
		case FORWARD:
			if(GameBoard[nextPosition.z - 1][nextPosition.x] != WALL)
				newDirection = PAC_Direction;
			break;
		case BACKWARD:
			if(GameBoard[nextPosition.z + 1][nextPosition.x] != WALL)
				newDirection = PAC_Direction;
			break;
		case LEFT:
			if(GameBoard[nextPosition.z][nextPosition.x - 1] != WALL)
				newDirection = PAC_Direction;
			break;
		case RIGHT:
			if(GameBoard[nextPosition.z][nextPosition.x + 1] != WALL)
				newDirection = PAC_Direction;
			break;
	}

	movePacman(newDirection);

	glutTimerFunc(speed, PAC_Update, 0);
}

void PAC_Animation(int value) {
	static int frame = 0;
	static bool up = true;

	if(up) {
		PacmanModel = PacmanAnimation[frame++];

		if(frame == PAC_ANIM_FRAMES) {
			up = false;
			frame -= 2;
		}

	}
	if(!up) {
		PacmanModel = PacmanAnimation[frame--];

		if(frame < 0) {
			up = true;
			frame = 1;
		}
	}

	glutTimerFunc(animationSpeed, PAC_Animation, 0);
}

void Ghost_Update(int value) {

	moveGhosts();

	glutTimerFunc(ghostSpeed, Ghost_Update, 0);
}

// Refresh the display at 50Hz
void refresh() {
	glutPostRedisplay();

	glutTimerFunc(20, refresh, 0);
}
