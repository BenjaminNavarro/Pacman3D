/*
 * gameFunctions.c
 *
 *  Created on: 20 oct. 2012
 *      Author: blackswords
 */

#include "gameFunctions.h"

gridPosition	nextPosition = {14,17};				// Next Pacman position
bool 			PacmanMoving = false;				// Tells if Pacman is moving or not
GLMmodel*		fruitsModels[FRUIT_COUNT];
GLMmodel* 		fruitModel;

ghost			ghosts[GHOST_COUNT];
pac				Pacman;

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
	if(( dx <= (PAC_PosInc*1.1f)) && (dz <= (PAC_PosInc*1.1f)))
		return true;

	return false;
}

/**
 * @brief  This function handles the moves of Pacman
 * @param  The direction where Pacman should go
 * @retval None
 */
void movePacman(direction dir) {

	static bool wait[GHOST_COUNT] = {false};

	// Locate Pacman on the grid
	Pacman.currentPosition = locateOnGrid(Pacman.position);

	// Test if the next cell is a correct one, if it's not Pacman is stopped
	switch (dir) {
	case NONE:
		dir = NONE;
		break;
	case FORWARD:
		if(wallOnTop(Pacman.currentPosition))
			dir = NONE;
		break;
	case BACKWARD:
		if(wallOnBottom(Pacman.currentPosition))
			dir = NONE;
		break;
	case LEFT:
		if(wallOnLeft(Pacman.currentPosition))
			dir = NONE;
		break;
	case RIGHT:
		if(wallOnRight(Pacman.currentPosition))
			dir = NONE;
		break;
	}

	// Test if Pacman is moving
	if(!Pacman.moving) {

		// Find the next cell to reach
		if(dir == NONE) {
			Pacman.currentDirection = NONE;
			Pacman.nextPosition = Pacman.currentPosition;
			return;
		}

		Pacman.nextPosition = findNextCell(Pacman.currentPosition, dir);

		// Left teleportation gate
		if(Pacman.nextPosition.x == 0) {
			Pacman.position.x = GameBaseSize.x;
			Pacman.nextPosition.x = N_CELLS_W-2;
		}

		// Right teleportation gate
		if(Pacman.nextPosition.x == N_CELLS_W-1) {
			Pacman.position.x = -GameBaseSize.x;
			Pacman.nextPosition.x = 1;
		}

		Pacman.currentDirection = dir;
		Pacman.moving = true;

		if((Pacman.previousDirection != Pacman.currentDirection)) {	// Pacman's direction changed

			for(int i=0; i< GHOST_COUNT; i++) {

				if(ghosts[i].PacmanMoves.numMoves == 0)
					wait[i] = false;

				if((ghosts[i].followMode == true) && (wait[i] == false) && (Pacman.currentDirection != NONE)) {
					if(isAPossibleNextMove(i)) {
						addMove(i,Pacman.currentPosition,Pacman.currentDirection);

						if(ghosts[i].PacmanMoves.numMoves == (level-1))
							wait[i] = true;

						printf("add move to %d : dir=%d at %d,%d\n",i,Pacman.currentDirection,Pacman.currentPosition.x,Pacman.currentPosition.z);
					}
				}

				if(Pacman.currentDirection == NONE)
					wait[i]=true;
			}
			Pacman.previousDirection = Pacman.currentDirection;
		}
	}
	else {
		// Move Pacman of PAC_PosInc in the correct direction
		switch (Pacman.currentDirection) {
		case NONE:
			// Nothing to do
			break;
		case FORWARD:
			Pacman.position.z -= PAC_PosInc;
			break;
		case BACKWARD:
			Pacman.position.z += PAC_PosInc;
			break;
		case LEFT:
			Pacman.position.x -= PAC_PosInc;
			break;
		case RIGHT:
			Pacman.position.x += PAC_PosInc;
			break;
		}

		// If the next position is reach Pacman is stopped and we check
		// if there is some action to do (take a coin for example)
		if(onCellCenter(Pacman.position, Pacman.nextPosition)) {
			Pacman.moving = false;
			checkCellAction(Pacman.nextPosition);
		}
	}
}

/**
 * @brief  This function handles the moves of the ghosts
 * @param  None
 * @retval None
 */
void moveGhosts() {

	for(int i=0; i<GHOST_COUNT; i++) {

		if(ghosts[i].atHome)
			continue;

		assert(isValidPosition(ghosts[i].position));

		// Locate the ghost on the grid
		ghosts[i].currentPosition = locateOnGrid(ghosts[i].position);

		assert(isValidCell(ghosts[i].currentPosition));

		// Test if the ghost is moving
		if(!ghosts[i].moving) {

			// Left teleportation gate
			if(ghosts[i].nextPosition.z == 14 && ghosts[i].nextPosition.x == 0) {
				ghosts[i].position.x = GameBaseSize.x;
				ghosts[i].nextPosition.x = N_CELLS_W-2;
			}

			// Right teleportation gate
			if(ghosts[i].nextPosition.z == 14 && ghosts[i].nextPosition.x == N_CELLS_W-1) {
				ghosts[i].position.x = -GameBaseSize.x;
				ghosts[i].nextPosition.x = 1;
			}

			ghosts[i].moving = true;
		}
		else {
			// Move the ghost of Ghost_PosInc in the correct direction

			// If the ghost is not blue but we are still in combo time, move the ghost twice faster
			if(!ghosts[i].blue && (comboTimeRemaining > 0))
				Ghost_PosInc *= 2.0;

			switch (ghosts[i].currentDirection) {
			case NONE:
				break;
			case FORWARD:
				ghosts[i].position.z -= Ghost_PosInc;
				break;
			case BACKWARD:
				ghosts[i].position.z += Ghost_PosInc;
				break;
			case LEFT:
				ghosts[i].position.x -= Ghost_PosInc;
				break;
			case RIGHT:
				ghosts[i].position.x += Ghost_PosInc;
				break;
			}

			// Restore the Ghost_PosInc to it's initial value when needed
			if(!ghosts[i].blue && (comboTimeRemaining > 0))
				Ghost_PosInc /= 2.0;

			if(!isValidPosition(ghosts[i].position)) {
				printf("%d is out at %d,%d\nYou are at %d,%d\n",i,ghosts[i].currentPosition.x,ghosts[i].currentPosition.z,Pacman.currentPosition.x,Pacman.currentPosition.z);
				assert(FAILED);
			}

			// If the next position is reach Pacman is stopped and we check
			// if there is some action to do (take a coin for example)
			if(onCellCenter(ghosts[i].position, ghosts[i].nextPosition)) {
				ghosts[i].position = gridToPos(ghosts[i].currentPosition);

				ghosts[i].moving = false;

				if(PacmanInSight(i) && !ghosts[i].blue) {

					ghosts[i].followMode = true;
					ghosts[i].cellToReach = Pacman.currentPosition;
					ghosts[i].lastPacmanDirection = Pacman.currentDirection;

					ghosts[i].nextPosition = findNextCell(ghosts[i].currentPosition, ghosts[i].currentDirection);

					ghosts[i].currentDirection = findDirection(ghosts[i].currentPosition, ghosts[i].nextPosition);

					printf("%d is following you! Go to %d,%d\n",i,ghosts[i].cellToReach.x,ghosts[i].cellToReach.z);

					clearMoves(i);

				}
				else if(ghosts[i].followMode == true) {
					// Reaching the last Pacman's position known
					if(!onCellCenter(ghosts[i].position, ghosts[i].cellToReach)) {

						ghosts[i].nextPosition = findNextCell(ghosts[i].currentPosition, ghosts[i].currentDirection);
						ghosts[i].currentDirection = findDirection(ghosts[i].currentPosition, ghosts[i].nextPosition);

					}
					// Last Pacman's known position reached
					else {

						// No more recorded positions to reach, return in random mode
						if(ghosts[i].PacmanMoves.numMoves == 0) {
							ghosts[i].currentDirection = ghosts[i].lastPacmanDirection;

							findNextRandomMove(i);

							ghosts[i].followMode = false;

							printf("%d is no longer following you\n",i);
						}
						// there is at least one move recorded
						else {

							do {

								ghosts[i].cellToReach = ghosts[i].PacmanMoves.moves->position;
								ghosts[i].lastPacmanDirection = ghosts[i].PacmanMoves.moves->dir;

								removeFirstMove(i);

								ghosts[i].currentDirection = findDirection(ghosts[i].currentPosition,ghosts[i].cellToReach);

								if(ghosts[i].PacmanMoves.numMoves == 0)
									break;

							} while (ghosts[i].currentDirection == NONE);

							if(ghosts[i].currentDirection != NONE) {
								ghosts[i].nextPosition = findNextCell(ghosts[i].currentPosition, ghosts[i].currentDirection);
								printf("%d %d,%d -> %d,%d dir=%d\n",i,ghosts[i].currentPosition.x,ghosts[i].currentPosition.z,ghosts[i].cellToReach.x,ghosts[i].cellToReach.z,ghosts[i].currentDirection);
							}
							else {
								findNextRandomMove(i);

								ghosts[i].followMode = false;

								printf("%d is no longer following you\n",i);
							}
						}
					}
				}
				// Random moves
				else {
					findNextRandomMove(i);
				}
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

			case FRUIT:
				glPushMatrix();
				glTranslatef(position.x, OBJECTS_HEIGHT, position.z);
				//glmDraw(fruitModel, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
				glutSolidSphere(0.05,8,8);
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
		score += COIN_POINTS;
		coinsLeft--;
		coinsPicked++;

		GameBoard[grid.z][grid.x] = EMPTY;

		if(isFruitCoin(coinsPicked))
			addFruit();
		break;

	case BIGCOIN:
		GameBoard[grid.z][grid.x] = EMPTY;
		score += BIGCOIN_POINTS;

		for(int i=0;i<GHOST_COUNT;i++) {
			ghosts[i].blue = true;
			ghosts[i].followMode = false;
		}

		startCombo(COMBO_TIME);
		break;

	case FRUIT:
		switch(level) {
		case 1:
			score += 100;
			break;
		case 2:
			score += 200;
			break;
		case 3:
			// fall-throught
		case 4:
			score += 500;
			break;
		case 5:
			// fall-throught
		case 6:
			score += 700;
			break;
		case 7:
			// fall-throught
		case 8:
			score += 1000;
			break;
		case 9:
			// fall-throught
		case 10:
			score += 2000;
			break;
		case 11:
			// fall-throught
		case 12:
			score += 3000;
			break;
		default:
			score += 5000;
			break;
		}

		removeFruit();
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
	level = 1;
	lives = MAX_LIVES;
	ghostInit = true;
	startCombo(0);
	coinsPicked = 0;


	gameBoardInit();

	resetPositions();
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

	Pacman.position = home;
	Pacman.currentDirection = NONE;
	Pacman.currentPosition = grid;
	Pacman.moving = false;
	Pacman.nextDirection = NONE;
	Pacman.previousDirection = NONE;

}

/**
 * @brief  Moves the specified ghost to its original position
 * @param  The ghost number
 * @retval None
 */
void sendGhostHome(int ghostNum) {
	gridPosition grid = {12 + ghostNum, 14};
	point home = gridToPos(grid);

	ghosts[ghostNum].position = home;
	ghosts[ghostNum].atHome = true;
	ghosts[ghostNum].followMode = false;
	ghosts[ghostNum].currentDirection = NONE;
	ghosts[ghostNum].currentPosition = grid;
	ghosts[ghostNum].moving = false;
	ghosts[ghostNum].nextDirection = NONE;
	ghosts[ghostNum].nextPosition = grid;
	ghosts[ghostNum].previousDirection = NONE;
	ghosts[ghostNum].blue = false;

	clearMoves(ghostNum);

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
	gridPosition grid ={12 + ghostNum, 11};
	point out = gridToPos(grid);

	ghosts[ghostNum].currentPosition = grid;
	ghosts[ghostNum].position = out;
	ghosts[ghostNum].currentDirection = FORWARD;
	ghosts[ghostNum].followMode = false;
	ghosts[ghostNum].moving = false;
	ghosts[ghostNum].atHome = false;

	findNextRandomMove(ghostNum);
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
	// and so the previous timer has to be ended
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
	if((value == COMBO_TIME) && !quitNext) {
		ghostSpeed *= 2;
	}

	// The combo period is ended
	else if((value == 0) && !quitNext) {
		comboValue = 200;

		for(int i=0;i<GHOST_COUNT;i++)
			ghosts[i].blue = false;

		lastValue = COMBO_TIME+1;

		ghostSpeed /= 2;
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
	for(int i=0; i<GHOST_COUNT; i++) {
		if(fabs(Pacman.position.x - ghosts[i].position.x) < (PAC_RADIUS + GHOST_SCALE)/2.0 && fabs(Pacman.position.z - ghosts[i].position.z) < (PAC_RADIUS + GHOST_SCALE)/2.0) {
			if(ghosts[i].blue == false) {
				lives--;
				if(lives == 0)
					gameOver();

				return;
			}
			else {
				sendGhostHome(i);
				score += comboValue;
				comboValue *= 2;
			}
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
	coinsPicked = 0;

//	if(ghostSpeed > 1) {
//		ghostSpeed--;
//		speed--;
//	}
}

/**
 * @brief  Add a fruit on the board depending on the level
 * @param  None
 * @retval None
 */
void addFruit() {
	float TTL = 5000.0 + 5000.0 * (float)rand()/(float)RAND_MAX;	// The fruit stays between 5s and 10s

	if(level > 13 && coinsPicked > 70)	// Only one key per level for levels > 13
		return;

	switch(level) {
	case 1:
		fruitModel = fruitsModels[Cherry];
		break;
	case 2:
		fruitModel = fruitsModels[Strawberry];
		break;
	case 3:
		// fall-throught
	case 4:
		fruitModel = fruitsModels[Orange];
		break;
	case 5:
		// fall-throught
	case 6:
		fruitModel = fruitsModels[Apple];
		break;
	case 7:
		// fall-throught
	case 8:
		fruitModel = fruitsModels[Grape];
		break;
	case 9:
		// fall-throught
	case 10:
		fruitModel = fruitsModels[IceCompot];
		break;
	case 11:
		// fall-throught
	case 12:
		fruitModel = fruitsModels[StewedFruit];
		break;
	default:
		fruitModel = fruitsModels[Key];
		break;
	}

	GameBoard[fruitPosition.z][fruitPosition.x] |= FRUIT;

	glutTimerFunc((int)TTL, removeFruit, 0);
}

/**
 * @brief  Removes the fruit from the game board
 * @param  None
 * @retval None
 */
void removeFruit() {
	GameBoard[fruitPosition.z][fruitPosition.x] &= ~FRUIT;
}

/**
 * @brief  Init the Pacman's moves list
 * @param  None
 * @retval None
 */
void initMoves() {
	for(int i=0; i<GHOST_COUNT; i++) {
		ghosts[i].PacmanMoves.numMoves = 0;
		ghosts[i].PacmanMoves.moves = NULL;
	}
}

/**
 * @brief  Add a move to the Pacman's moves list
 * @param  pos : Position where the move occured
 * #param	newDirection : the direction Pacman took
 * @retval None
 */
void addMove(int ghost, gridPosition pos, direction newDirection) {
	move* newMove;

	newMove = (move*) malloc(sizeof(move));

	if(newMove == NULL) {
		printf("Memory allocation failure\n");
		exit(EXIT_FAILURE);
	}

	newMove->dir 		= newDirection;
	newMove->position	= pos;
	newMove->next 		= NULL;

	if(ghosts[ghost].PacmanMoves.moves == NULL) {
		ghosts[ghost].PacmanMoves.moves = newMove;
	}
	else {
		move* new = ghosts[ghost].PacmanMoves.moves;
		while(new->next != NULL)
			new = new->next;

		new->next = newMove;
	}

	(ghosts[ghost].PacmanMoves.numMoves)++;
}

/**
  * @brief  Removes the first move in the ghost's recorded moves
  * @param  The ghost number
  * @retval None
  */
void removeFirstMove(int ghost) {
	if(ghosts[ghost].PacmanMoves.numMoves > 0) {
		move* first = ghosts[ghost].PacmanMoves.moves;
		move* second = first->next;

		free(first);

		ghosts[ghost].PacmanMoves.moves = second;
		(ghosts[ghost].PacmanMoves.numMoves)--;
	}
}

bool isAPossibleNextMove(int i) {

	move* last;
	bool ok = false;
	int xinc,zinc;
	gridPosition currentPos;

	if(ghosts[i].PacmanMoves.numMoves == 0) {
		move ghost;
		ghost.dir = ghosts[i].currentDirection;
		ghost.position = ghosts[i].currentPosition;

		last = &ghost;
	}
	else {
		last=ghosts[i].PacmanMoves.moves;

		while(last->next != NULL)
			last = last->next;
	}

	switch(last->dir) {
	case FORWARD:
		if(last->position.z >= Pacman.currentPosition.z)
			xinc = 0;
			zinc = -1;
		break;
	case BACKWARD:
		if(last->position.z <= Pacman.currentPosition.z)
			xinc = 0;
			zinc = 1;
		break;
	case LEFT:
		if(last->position.x >= Pacman.currentPosition.x)
			xinc = -1;
			zinc = 0;
		break;
	case RIGHT:
		if(last->position.x <= Pacman.currentPosition.x)
			xinc = 1;
			zinc = 0;
		break;
	default:
		printf("crap\n");
		assert(0);
		break;
	}

	if(ok == false)
		return false;

	currentPos = last->position;

	do {
		currentPos.x += xinc;
		currentPos.z += zinc;
		if(currentPos.x == Pacman.currentPosition.x && currentPos.z == Pacman.currentPosition.z) {
			return true;
		}
		if(currentPos.x < 0 || currentPos.x > N_CELLS_W) {	// no walls for the teleportation gates
			break;
		}
		if(currentPos.z < 0 || currentPos.z > N_CELLS_H) {
			break;
		}

	} while(GameBoard[currentPos.z][currentPos.x] != WALL);

	return false;
}

/**
 * @brief  Clear the Pacman's moves list
 * @param  None
 * @retval None
 */
void clearMoves(int ghost) {
	if(ghosts[ghost].PacmanMoves.numMoves == 0)
		return;

	move* pt = ghosts[ghost].PacmanMoves.moves;
	move* toDelete;

	while(pt != NULL) {
		toDelete = pt;
		pt = pt->next;
		free(toDelete);
	}

	ghosts[ghost].PacmanMoves.moves = NULL;
	ghosts[ghost].PacmanMoves.numMoves = 0;
}

bool PacmanInSight(int ghostNum) {
	gridPosition currentPos,PacmanPos = locateOnGrid(Pacman.position);
	int xinc, zinc;

	if(level == 1)
		return false;

	switch(ghosts[ghostNum].currentDirection) {
		case FORWARD:
			zinc = -1;
			xinc = 0;
			break;
		case BACKWARD:
			zinc = 1;
			xinc = 0;
			break;
		case LEFT:
			zinc = 0;
			xinc = -1;
			break;
		case RIGHT:
			zinc = 0;
			xinc = 1;
			break;
		case NONE:
			break;
		}

		currentPos.x = ghosts[ghostNum].currentPosition.x;
		currentPos.z = ghosts[ghostNum].currentPosition.z;

		do {
			currentPos.x += xinc;
			currentPos.z += zinc;
			if(currentPos.x == PacmanPos.x && currentPos.z == PacmanPos.z) {
				return true;
			}
			if(currentPos.x < 0 || currentPos.x > N_CELLS_W) {	// no walls for the teleportation gates
				break;
			}
			if(currentPos.z < 0 || currentPos.z > N_CELLS_H) {
				break;
			}

		} while(GameBoard[currentPos.z][currentPos.x] != WALL);

	return false;
}

gridPosition findNextCell(gridPosition currentPosition, direction dir) {
	gridPosition nextPosition;

	switch (dir) {
	case NONE:
		nextPosition.x = currentPosition.x;
		nextPosition.z = currentPosition.z;
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

	return nextPosition;
}

void findNextRandomMove(int ghost) {
		bool test = false;
		direction temp;
		float r;

		switch(ghosts[ghost].currentDirection) {
		case NONE:
			// fall-through
		case FORWARD:
			while(test == false) {
				r = (float) rand() / (float) RAND_MAX;

				if((r < 0.33) && !wallOnLeft(ghosts[ghost].currentPosition)){
					temp = LEFT;
				}
				else if((r < 0.66)  && !wallOnTop(ghosts[ghost].currentPosition)) {
					temp = FORWARD;
				}
				else if((r >= 0.66) && !wallOnRight(ghosts[ghost].currentPosition)) {
					temp = RIGHT;
				}
				else
					continue;

				if(temp != NONE)
					test = true;
			}
			break;
		case BACKWARD:
			while(test == false) {
				r = (float) rand() / (float) RAND_MAX;

				if((r < 0.33) && !wallOnLeft(ghosts[ghost].currentPosition)){
					temp = LEFT;
				}
				else if((r < 0.66)  && !wallOnBottom(ghosts[ghost].currentPosition)) {
					temp = BACKWARD;
				}
				else if((r >= 0.66) && !wallOnRight(ghosts[ghost].currentPosition)) {
					temp = RIGHT;
				}
				else
					continue;

				if(temp != NONE)
					test = true;
			}
			break;
		case LEFT:
			while(test == false) {
				r = (float) rand() / (float) RAND_MAX;

				if((r < 0.33) && !wallOnBottom(ghosts[ghost].currentPosition)){
					temp = BACKWARD;
				}
				else if((r < 0.66)  && !wallOnTop(ghosts[ghost].currentPosition)) {
					temp = FORWARD;
				}
				else if((r >= 0.66) && !wallOnLeft(ghosts[ghost].currentPosition)) {
					temp = LEFT;
				}
				else
					continue;

				if(temp != NONE)
					test = true;
			}
			break;
		case RIGHT:
			while(test == false) {
				r = (float) rand() / (float) RAND_MAX;

				if((r < 0.33) && !wallOnBottom(ghosts[ghost].currentPosition)){
					temp = BACKWARD;
				}
				else if((r < 0.66)  && !wallOnTop(ghosts[ghost].currentPosition)) {
					temp = FORWARD;
				}
				else if((r >= 0.66) && !wallOnRight(ghosts[ghost].currentPosition)) {
					temp = RIGHT;
				}
				else
					continue;

				if(temp != NONE)
					test = true;
			}
			break;
		}

		ghosts[ghost].currentDirection = temp;

		if(ghosts[ghost].currentDirection == NONE)
			assert(FAILED);

		// Find the next cell to reach
		ghosts[ghost].nextPosition = findNextCell(ghosts[ghost].currentPosition, ghosts[ghost].currentDirection);

		assert(isValidCell(ghosts[ghost].nextPosition));
		assert(isOnWall(ghosts[ghost].nextPosition));

}

/**
  * @brief  Finds the correct direction to reach the next point
  * @param  current : current position
  * @param  next 	: position to reach
  * @retval Direction to take, NONE if it's not a reachable position
  */
direction findDirection(gridPosition current, gridPosition next) {
	direction dir;

	if ((current.x < next.x) && (current.z == next.z))
		dir = RIGHT;
	else if ((current.x > next.x) && (current.z == next.z))
		dir = LEFT;
	else if ((current.x == next.x) && (current.z < next.z))
		dir = BACKWARD;
	else if ((current.x == next.x) && (current.z > next.z))
		dir = FORWARD;
	else	// not a reachable position
		dir = NONE;

	return dir;
}
