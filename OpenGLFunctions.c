#include "OpenGLFunctions.h"

/***				Global variables			***/
GLMmodel*  		PacmanModel;						// glm model data structure for pacman
GLMmodel*		PacmanAnimation[PAC_ANIM_FRAMES];	// array of the different pacman models
GLMmodel*		Ghost[GHOST_COUNT];					// array of the different ghosts models
GLMmodel*		BlueGhost;							// Model for the blue ghosts (when haunted)
GLMmodel*		cherry;								// Model for the cherry
GLuint			floorTex;							// handle for the floor texture
float 			camAngle = 45.0f;					// The angle of the camera
direction 		newDirection = NONE;				// Pacman's new direction

extern ghost	ghosts[GHOST_COUNT];
extern pac		Pacman;



//Draws the 3D scene
void drawScene() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 3D mode? Yes...
	if(camAngle != 90.0) {
		// translate the camera to follow pacman
		// Projection of pacman's z coordinate on the y and z axes depending on the camera angle
		glTranslatef(-Pacman.position.x, Pacman.position.z*sin(degToRad(camAngle)), -2.5f - Pacman.position.z*cos(degToRad(camAngle)));
		glRotatef(camAngle,1,0,0);
	}
	// No... 2D-like mode
	else {
		glTranslatef(0, -0.5, -7);
		glRotatef(camAngle,1,0,0);
	}

	// Enable the use of 2D textures
	glEnable(GL_TEXTURE_2D);

		// Load the floor texture
		glBindTexture(GL_TEXTURE_2D, floorTex);

		// Set the texture rendering to linear
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Draw the bottom base square and map the texture
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

		glTranslatef(Pacman.position.x, PAC_RADIUS, Pacman.position.z);
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
		direction dir;
		glPushMatrix();

		glTranslatef(ghosts[i].position.x, GHOST_SCALE, ghosts[i].position.z);
		glScalef(GHOST_SCALE,GHOST_SCALE,GHOST_SCALE);

		// If the ghost is stopped, we take its previous direction
		if(ghosts[i].currentDirection == NONE)
			dir = ghosts[i].previousDirection;
		else
			dir = ghosts[i].currentDirection;


		switch (dir) {
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

		// If the ghosts are not blue, we load their respective models, else we load the blue ghost model
		if(ghosts[i].blue == false)
			glmDraw(Ghost[i], GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
		else
			glmDraw(BlueGhost, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);

		glPopMatrix();

	}

	// Display the HUD on the screen
	displayHUD();

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

	}

	BlueGhost = glmReadOBJ("models/ghost_blue.obj");
	glmUnitize(BlueGhost);
	glmFacetNormals(BlueGhost);
	glmVertexNormals(BlueGhost,90.0f);

	cherry = glmReadOBJ("models/cherry.obj");
	glmUnitize(cherry);
	glmFacetNormals(cherry);
	glmVertexNormals(cherry,90.0f);

	// Set PacmanModel pointing to the first frame
	PacmanModel = PacmanAnimation[0];

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	resetPositions();

}

//Called when a key is pressed
void handleKeypress(unsigned char key, int x, int y) {

	switch (tolower(key)) {
	case 27: //Escape key
		glDeleteTextures(1,&floorTex);
		exit(EXIT_SUCCESS);
		break;
	case 'a':	// Stop (for testing purposes)
		Pacman.nextDirection = NONE;
		break;
	case 'z':	// Move forward
		Pacman.nextDirection = FORWARD;
		break;
	case 's':	// Move backward
		Pacman.nextDirection = BACKWARD;
		break;
	case 'q':	// Move left
		Pacman.nextDirection = LEFT;
		break;
	case 'd':	// Move right
		Pacman.nextDirection = RIGHT;
		break;
	case 'p':	// Move low
		camAngle += 1;
		break;
	case 'm':	// Move low
		camAngle -= 1;
		break;
	case '2':	// Move low
		camAngle = 90.0f;
		break;
	case '3':	// Move low
		camAngle = 45.0f;
		break;
	}
}

//Called when a key is released
void handleKeyup(unsigned char key, int x, int y) {
	switch (key) {
	case 'h':	// Move high
		Pacman.position.y = 0;
		break;
	}
}

//Initializes 3D renderingy
void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); //Enable alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blend function
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	gameBoardInit();
	initMoves();

	Image* floor = loadBMP("images/floor.bmp");
	floorTex = loadTexture(floor);

	loadModels();

	t3dInit();

	srand(time(NULL));

	glutTimerFunc(20, refresh, 0);						//Add the screen refresh timer
	glutTimerFunc(speed, PAC_Update, 0); 				//Add the Pacman timer
	glutTimerFunc(animationSpeed, PAC_Animation, 0); 	//Add the Pacman animation timer
	glutTimerFunc(ghostSpeed, Ghost_Update, 0); 		//Add the ghosts timer
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
	if(Pacman.nextDirection != NONE)
		nextPosition = Pacman.nextPosition;
	else
		nextPosition = locateOnGrid(Pacman.position);

	switch (Pacman.nextDirection) {
		case NONE:
			newDirection = NONE;
			break;
		case FORWARD:
			if(!wallOnTop(nextPosition))
				newDirection = Pacman.nextDirection;
			break;
		case BACKWARD:
			if(!wallOnBottom(nextPosition))
				newDirection = Pacman.nextDirection;
			break;
		case LEFT:
			if(!wallOnLeft(nextPosition))
				newDirection = Pacman.nextDirection;
			break;
		case RIGHT:
			if(!wallOnRight(nextPosition))
				newDirection = Pacman.nextDirection;
			break;
	}

	movePacman(newDirection);

	// Check if Pacman is touching a ghost
	checkGhosts();

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

// Refresh the display
void refresh() {
	glutPostRedisplay();

	glutTimerFunc(40, refresh, 0);
}
