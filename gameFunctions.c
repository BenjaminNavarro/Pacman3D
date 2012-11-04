/*
 * gameFunctions.c
 *
 *  Created on: 20 oct. 2012
 *      Author: blackswords
 */

#include "gameFunctions.h"

gridPosition	nextPosition = {14,17};				// Next Pacman position
bool 			ghostMoving[GHOST_COUNT] = {false};	// Tells if the ghost is moving or not
bool 			PacmanMoving = false;				// Tells if Pacman is moving or not
bool			ghostHome[GHOST_COUNT] = {false};


/**
 * @brief  Finds the grid's cell corresponding to the given position
 * @param  The position (x,z) on the game's boad
 * @retval the cell coordinate (0,0 is the top left corner)
 */
gridPosition locateOnGrid(point position) {
	gridPosition grid;

	point loc;

	loc.x = (position.x + GameBaseSize.x) / (2.0f*GameBaseSize.x);		// Normalize to 0-1
	loc.x *= (float) N_CELLS_W;											// Normalize to 0-N_cellS_W-1

	loc.z = (position.z + GameBaseSize.z) / (2.0f*GameBaseSize.z);		// Normalize to 0-1
	loc.z *= (float) N_CELLS_H;											// Normalize to 0-N_cellS_H-1

	grid.x = (unsigned int) floor(loc.x);								// Pick the lowest nearest integer
	grid.z = (unsigned int) floor(loc.z);								// Pick the lowest nearest integer

	return grid;
}

/**
 * @brief  Gives the position of the given grid's cell
 * @param  The cell coordinate
 * @retval The position (x,z) of the cell
 */
point gridToPos(gridPosition grid) {
	point position;

	// Calculation of the cell width and height;
	float cellWidth = 2.0f * GameBaseSize.x / (float) N_CELLS_W;
	float cellHeight = 2.0f * GameBaseSize.z / (float) N_CELLS_H;

	// Gives the position between 0 and 2*GameBaseSize
	position.x = cellWidth * ( 0.5f + (float) grid.x);
	position.z = cellHeight * (0.5f + (float) grid.z);

	// Gives the position between -GameBaseSize and GameBaseSize
	position.x -= GameBaseSize.x;
	position.z -= GameBaseSize.z;

	return position;
}

/**
 * @brief  Tells if the given position is corresponding to the cell center
 * @param  The position to test
 * @param	The cell to test
 * @retval True if the position is on a cell center, false otherwise
 */
bool onCellCenter(point position, gridPosition cell) {
	point centerPos = gridToPos(cell);
	float dx, dz;

	// Difference between the actual position and the position to reach
	dx = fabs(position.x - centerPos.x);
	dz = fabs(position.z - centerPos.z);

	// If dx and dz are lower than the position increment we consider
	// that the center is reached
	if(( dx <= PAC_PosInc) && (dz <= PAC_PosInc))
		return true;

	return false;
}

/**
 * @brief  This function handles the moves of Pacman
 * @param  The direction where Pacman should go
 * @retval None
 */
void movePacman(direction dir) {
	static gridPosition	currentPosition;
	static direction	currentDirection = NONE;

	// Locate Pacman on the grid
	currentPosition = locateOnGrid(PAC_Position);

	// Test if the next cell is a correct one, if it's not Pacman is stopped
	switch (dir) {
	case NONE:
		currentDirection = NONE;
		break;
	case FORWARD:
		if(GameBoard[currentPosition.z - 1][currentPosition.x] == WALL)
			dir = NONE;
		break;
	case BACKWARD:
		if(GameBoard[currentPosition.z + 1][currentPosition.x] == WALL)
			dir = NONE;
		break;
	case LEFT:
		if(GameBoard[currentPosition.z][currentPosition.x - 1] == WALL)
			dir = NONE;
		break;
	case RIGHT:
		if(GameBoard[currentPosition.z][currentPosition.x + 1] == WALL)
			dir = NONE;
		break;
	}

	// Test if Pacman is moving
	if(!PacmanMoving) {

		// Find the next cell to reach
		switch (dir) {
		case NONE:
			currentDirection = NONE;
			nextPosition.x = currentPosition.x;
			nextPosition.z = currentPosition.z;
			return;
			break;
		case FORWARD:
			nextPosition.x = currentPosition.x;
			nextPosition.z = currentPosition.z - 1;
			break;
		case BACKWARD:
			nextPosition.x = currentPosition.x;
			nextPosition.z = currentPosition.z + 1;
			break;
		case LEFT:
			nextPosition.x = currentPosition.x - 1;
			nextPosition.z = currentPosition.z;
			break;
		case RIGHT:
			nextPosition.x = currentPosition.x + 1;
			nextPosition.z = currentPosition.z;
			break;
		}

		// Left teleportation gate
		if(nextPosition.x == 0) {
			PAC_Position.x = GameBaseSize.x;
			nextPosition.x = N_CELLS_W-2;
		}

		// Right teleportation gate
		if(nextPosition.x == N_CELLS_W-1) {
			PAC_Position.x = -GameBaseSize.x;
			nextPosition.x = 1;
		}

		currentDirection = dir;
		PacmanMoving = true;
	}
	else {
		// Move Pacman of PAC_PosInc in the correct direction
		switch (currentDirection) {
		case NONE:
			// Nothing to do
			break;
		case FORWARD:
			PAC_Position.z -= PAC_PosInc;
			break;
		case BACKWARD:
			PAC_Position.z += PAC_PosInc;
			break;
		case LEFT:
			PAC_Position.x -= PAC_PosInc;
			break;
		case RIGHT:
			PAC_Position.x += PAC_PosInc;
			break;
		}

		// If the next position is reach Pacman is stopped and we check
		// if there is some action to do (take a coin for example)
		if(onCellCenter(PAC_Position, nextPosition)) {
			PacmanMoving = false;
			checkCellAction(nextPosition);
		}
	}

}

/**
 * @brief  This function handles the moves of the ghosts
 * @param  None
 * @retval None
 */
void moveGhosts() {
	static gridPosition	currentPosition[GHOST_COUNT];
	static gridPosition	nextPosition[GHOST_COUNT];
	static direction	currentDirection[GHOST_COUNT] = {NONE};
	static direction	previousDirection[GHOST_COUNT] = {NONE};
	float r;


	for(int i=0; i<GHOST_COUNT; i++) {

		if(ghostHome[i])
			continue;

		// Locate the ghost on the grid
		currentPosition[i] = locateOnGrid(Ghost_Position[i]);

		// Test if the next cell is a correct one, if it's not the ghost is sent to a random direction
		switch (Ghost_Direction[i]) {
		case NONE:
			do {
				r = (float) rand() / (float) RAND_MAX;
				if(r < 0.25)
					Ghost_Direction[i] = FORWARD;
				else if(r < 0.5)
					Ghost_Direction[i] = BACKWARD;
				else if(r < 0.75)
					Ghost_Direction[i] = LEFT;
				else
					Ghost_Direction[i] = RIGHT;
			} while( Ghost_Direction[i] == previousDirection[i]);
			break;
		case FORWARD:
			if(GameBoard[currentPosition[i].z - 1][currentPosition[i].x] == WALL) {
				previousDirection[i] = FORWARD;
				Ghost_Direction[i] = NONE;
			}
			break;
		case BACKWARD:
			if(GameBoard[currentPosition[i].z + 1][currentPosition[i].x] == WALL) {
				previousDirection[i] = BACKWARD;
				Ghost_Direction[i] = NONE;
			}
			break;
		case LEFT:
			if(GameBoard[currentPosition[i].z][currentPosition[i].x - 1] == WALL) {
				previousDirection[i] = LEFT;
				Ghost_Direction[i] = NONE;
			}
			break;
		case RIGHT:
			if(GameBoard[currentPosition[i].z][currentPosition[i].x + 1] == WALL) {
				previousDirection[i] = RIGHT;
				Ghost_Direction[i] = NONE;
			}
			break;
		}

		// Test if the ghost is moving
		if(!ghostMoving[i]) {

			// Find the next cell to reach
			switch (Ghost_Direction[i]) {
			case NONE:
				return;
				break;
			case FORWARD:
				nextPosition[i].x = currentPosition[i].x;
				nextPosition[i].z = currentPosition[i].z - 1;
				break;
			case BACKWARD:
				nextPosition[i].x = currentPosition[i].x;
				nextPosition[i].z = currentPosition[i].z + 1;
				break;
			case LEFT:
				nextPosition[i].x = currentPosition[i].x - 1;
				nextPosition[i].z = currentPosition[i].z;
				break;
			case RIGHT:
				nextPosition[i].x = currentPosition[i].x + 1;
				nextPosition[i].z = currentPosition[i].z;
				break;
			}

			// Left teleportation gate
			if(nextPosition[i].x == 0) {
				Ghost_Position[i].x = GameBaseSize.x;
				nextPosition[i].x = N_CELLS_W-2;
			}

			// Right teleportation gate
			if(nextPosition[i].x == N_CELLS_W-1) {
				Ghost_Position[i].x = -GameBaseSize.x;
				nextPosition[i].x = 1;
			}

			currentDirection[i] = Ghost_Direction[i];
			ghostMoving[i] = true;
		}
		else {
			// Move the ghost of Ghost_PosInc in the correct direction
			switch (currentDirection[i]) {
			case NONE:
				// Nothing to do
				break;
			case FORWARD:
				Ghost_Position[i].z -= Ghost_PosInc;
				break;
			case BACKWARD:
				Ghost_Position[i].z += Ghost_PosInc;
				break;
			case LEFT:
				Ghost_Position[i].x -= Ghost_PosInc;
				break;
			case RIGHT:
				Ghost_Position[i].x += Ghost_PosInc;
				break;
			}

			// If the next position is reach Pacman is stopped and we check
			// if there is some action to do (take a coin for example)
			if(onCellCenter(Ghost_Position[i], nextPosition[i])) {
				ghostMoving[i] = false;
				//checkCellAction(nextPosition[i]);
			}
		}

	}
}

/**
 * @brief  Draw the differents elements present on the game board
 * @param  None
 * @retval None
 */
void renderGame() {
	point 			position;
	gridPosition 	grid;

	glEnable(GL_COLOR_MATERIAL);

	for(int zcell=0; zcell<N_CELLS_H; zcell++) {
		for(int xcell=0; xcell<N_CELLS_W; xcell++) {
			grid.x = xcell;
			grid.z = zcell;
			position = gridToPos(grid);

			switch(GameBoard[zcell][xcell]) {
			case EMPTY:
				break;

			case COIN:
				glPushMatrix();
				glColorRGB(0xFC, 0xF2, 0x7B);									// Yellow
				glTranslatef(position.x, OBJECTS_HEIGHT, position.z);
				glutSolidSphere(COIN_RADIUS , 8 , 8 );
				glPopMatrix();
				break;

			case BIGCOIN:
				glPushMatrix();
				glColorRGB(0xFC, 0xF2, 0x7B);										// Yellow
				glTranslatef(position.x, OBJECTS_HEIGHT, position.z);
				glutSolidSphere(BIGCOIN_RADIUS , 16 , 16 );
				glPopMatrix();
				break;
			default:
				break;
			}
		}
	}

	glDisable(GL_COLOR_MATERIAL);
}

/**
 * @brief  Checks if something have to be done if Pacman is on the specified cell
 * @param  The cell to check
 * @retval None
 */
void checkCellAction(gridPosition grid) {

	switch(GameBoard[grid.z][grid.x]) {
	case EMPTY:
		break;

	case COIN:
		GameBoard[grid.z][grid.x] = EMPTY;
		score += COIN_POINTS;
		coinsLeft--;
		break;

	case BIGCOIN:
		GameBoard[grid.z][grid.x] = EMPTY;
		score += BIGCOIN_POINTS;
		hunted = false;
		startCombo(COMBO_TIME);
		break;
	default:
		break;
	}

	if(coinsLeft == 0)
		levelUp();

}

/**
 * @brief  Displays the HUD (score, lives) on the screen
 * @param  None
 * @retval None
 */
void displayHUD() {
	char textScore[20];
	char textLives[5];
	char textLevel[5];
	char textCombo[3];

	sprintf(textScore,"%d",score);		// Put the score into a string
	sprintf(textLives,"%d",lives);		// Put the lives number into a string
	sprintf(textLevel,"%d",level);		// Put the level number into a string


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, -1.0, -1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);

	glClear(GL_DEPTH_BUFFER_BIT);

		glEnable(GL_COLOR_MATERIAL);

		glPushMatrix();

			glColor3f(10,10,10);			// Bright white text
			glTranslatef(0.8, 0.9, 0);		// Move to the top right hand corner
			glScalef(0.08,0.1,0.1);			// Scale down the text

			t3dDraw2D("Score", 0, 0, 0);	// Put some decorative text

			glTranslatef(0, -1.5, 0);		// Translate the score under the previous text
			t3dDraw2D(textScore, 0, 0, 0);	// Draw the score

		glPopMatrix();

		glPushMatrix();

			glColor3f(10,10,10);			// Bright white text
			glTranslatef(0.5, 0.9, 0);		// Move to the top right hand corner, before the score
			glScalef(0.08,0.1,0.1);			// Scale down the text

			t3dDraw2D("Lives", 0, 0, 0);	// Put some decorative text

			glTranslatef(0, -1.5, 0);		// Translate the lives number under the previous text
			t3dDraw2D(textLives, 0, 0, 0);	// Draw the lives number

		glPopMatrix();

		glPushMatrix();

			glColor3f(10,10,10);			// Bright white text
			glTranslatef(-0.8, 0.9, 0);		// Move to the top left hand corner
			glScalef(0.08,0.1,0.1);			// Scale down the text

			t3dDraw2D("Level", 0, 0, 0);	// Put some decorative text

			glTranslatef(0, -1.5, 0);		// Translate the level number under the previous text
			t3dDraw2D(textLevel, 0, 0, 0);	// Draw the level number

		glPopMatrix();

		if(comboTimeRemaining > 0) {
			sprintf(textCombo,"%ds",comboTimeRemaining);		// Put the combo remaining time into a string

			glPushMatrix();

				glColor3f(10,10,10);			// Bright white text
				glTranslatef(0.15, 0.9, 0);		// Move to the top right hand corner, before the lives
				glScalef(0.08,0.1,0.1);			// Scale down the text

				t3dDraw2D("Combo", 0, 0, 0);	// Put some decorative text

				glTranslatef(0, -1.5, 0);		// Translate the lives number under the previous text
				t3dDraw2D(textCombo, 0, 0, 0);	// Draw the lives number

			glPopMatrix();
		}

		glDisable(GL_COLOR_MATERIAL);

	// Making sure we can render 3d again
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

}

/**
  * @brief  Gives the next Pacman position
  * @param  None
  * @retval Next Pacman position
  */
gridPosition getNextPosition() {
	return nextPosition;
}

/**
  * @brief  Moves Pacman and the ghosts to their original positions
  * @param  None
  * @retval None
  */
void resetPositions() {
	ghostInit = true;

	for(int i=0; i<GHOST_COUNT; i++)
		sendGhostHome(i);

	sendPacmanHome();
}

/**
  * @brief  Reset the game to its original state
  * @param  None
  * @retval None
  */
void gameOver() {
	score = 0;
	lives = MAX_LIVES;
	ghostInit = true;
	startCombo(0);

	gameBoardInit();

	sendPacmanHome();

	for(int i=0; i<GHOST_COUNT; i++)
		sendGhostHome(i);
}

/**
  * @brief  Moves Pacman to its original position
  * @param  The ghost number
  * @retval None
  */
void sendPacmanHome() {
	float cellWidth = 2.0f * GameBaseSize.x / (float) N_CELLS_W;
	gridPosition grid ={14,23};

	point home = gridToPos(grid);

	home.x -= cellWidth/2.0;

	PAC_Position = home;
	PAC_Direction = NONE;
	PacmanMoving = false;

}

/**
  * @brief  Moves the specified ghost to its original position
  * @param  The ghost number
  * @retval None
  */
void sendGhostHome(int ghostNum) {
	gridPosition grid = {12 + ghostNum, 14};
	point home = gridToPos(grid);

	Ghost_Position[ghostNum] = home;
	ghostHome[ghostNum] = true;

	if(ghostInit)
		glutTimerFunc(ghostNum*1000, sendGhostBoard, ghostNum);
	else
		glutTimerFunc(5000, sendGhostBoard, ghostNum);

	if(ghostNum == GHOST_COUNT-1)
		ghostInit = false;
}

/**
  * @brief  Moves the specifed ghost out its home
  * @param  The ghost number
  * @retval None
  */
void sendGhostBoard(int ghostNum) {
	gridPosition grid ={12 + ghostNum,11};
	point home = gridToPos(grid);

	Ghost_Position[ghostNum] = home;
	Ghost_Direction[ghostNum] = NONE;
	ghostMoving[ghostNum] = false;
	ghostHome[ghostNum] = false;
}

/**
  * @brief  Starts the combo period and updates the comboValue
  * @param  None
  * @retval None
  */
void startCombo(int value) {
	static int 	lastValue = COMBO_TIME+1;
	static bool quitNext = false;

	// if quitNext is set that means the the combo timer should be restarted
	// and so the previous timer has to end
	if(quitNext)  {
		quitNext = false;
		return;
	}

	// Detect if the timer is restarted to its original value
	if(value != lastValue-1)
		quitNext = true;

	lastValue = value;

	// Store the number of seconds remaining for the HUD
	comboTimeRemaining = value;

	// The combo period is started, Pacman is hunting!
	if(value == COMBO_TIME) {
		hunted = false;
	}

	// The combo period is ended
	else if(value == 0) {
		comboValue = 200;
		hunted = true;
		lastValue = COMBO_TIME+1;
	}

	// Some time left, calling startCombo in 1s with the remaining time
	if(value > 0) {
		glutTimerFunc(1000, startCombo, value-1);
	}

}

/**
  * @brief  Tests if Pacman hits a ghost and does the necessary actions
  * @param  None
  * @retval None
  */
void checkGhosts() {
	bool ghostTouched = false;
	int  ghostNum;

	for(int i=0; i<GHOST_COUNT; i++) {
		gridPosition ghostPos;
		ghostPos = locateOnGrid(Ghost_Position[i]);

		if(fabs(PAC_Position.x - Ghost_Position[i].x) < (PAC_RADIUS + GHOST_SCALE)/2.0 && fabs(PAC_Position.z - Ghost_Position[i].z) < (PAC_RADIUS + GHOST_SCALE)/2.0) {
			ghostTouched = true;
			ghostNum = i;
		}
	}

	if(ghostTouched) {
		if(hunted) {
			lives--;
			if(lives == 0)
				gameOver();

			resetPositions();
		}
		else {
			sendGhostHome(ghostNum);
			score += comboValue;
			comboValue *= 2;
		}
	}
}

/**
  * @brief  Intialize the content of GameBoard with GameBoardInit
  * @param  None
  * @retval None
  */
void gameBoardInit() {
	for(int zcell=0; zcell<N_CELLS_H; zcell++) {
		for(int xcell=0; xcell<N_CELLS_W; xcell++) {
			GameBoard[zcell][xcell] = GameBoardInit[zcell][xcell];

			if(GameBoard[zcell][xcell] == COIN)
				coinsLeft++;
		}
	}
}

/**
  * @brief  Performs the necessary action for a level up
  * @param  None
  * @retval None
  */
void levelUp() {
	gameBoardInit();
	resetPositions();
	level++;

	if(ghostSpeed > 1)
		ghostSpeed -= 2;
}
