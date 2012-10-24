/*
 * gameFunctions.c
 *
 *  Created on: 20 oct. 2012
 *      Author: blackswords
 */

#include "gameFunctions.h"

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
	static gridPosition	nextPosition;
	static bool 		moving = false;
	static direction	currentDirection = NONE;

	// Locate Pacman on the grid
	currentPosition = locateOnGrid(PAC_Position);

	// Test if the next cell is a correct one, if it's not Pacman is stopped
	switch (dir) {
	case NONE:
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
	if(!moving) {

		// Find the next cell to reach
		switch (dir) {
		case NONE:
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
		moving = true;
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
			moving = false;
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
	static bool 		moving[GHOST_COUNT] = {false};
	static direction	currentDirection[GHOST_COUNT] = {NONE};
	static direction	previousDirection[GHOST_COUNT] = {NONE};
	float r;


	for(int i=0; i<GHOST_COUNT; i++) {

		// Locate the ghost on the grid
		currentPosition[i] = locateOnGrid(Ghost_Position[i]);

		// Test if the next cell is a correct one, if it's not Pacman is stopped
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

		// Test if Pacman is moving
		if(!moving[i]) {

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
			moving[i] = true;
		}
		else {
			// Move the ghost of Ghost_PosInc in the correct direction
			switch (currentDirection[i]) {
			case NONE:
				// Nothing to do
				break;
			case FORWARD:
				Ghost_Position[i].z -= Ghost_PosInc[i];
				break;
			case BACKWARD:
				Ghost_Position[i].z += Ghost_PosInc[i];
				break;
			case LEFT:
				Ghost_Position[i].x -= Ghost_PosInc[i];
				break;
			case RIGHT:
				Ghost_Position[i].x += Ghost_PosInc[i];
				break;
			}

			// If the next position is reach Pacman is stopped and we check
			// if there is some action to do (take a coin for example)
			if(onCellCenter(Ghost_Position[i], nextPosition[i])) {
				moving[i] = false;
				checkCellAction(nextPosition[i]);
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
		break;

	case BIGCOIN:
		GameBoard[grid.z][grid.x] = EMPTY;
		score += BIGCOIN_POINTS;
		break;
	default:
		break;
	}
}

/**
 * @brief  Displays the score on the screen
 * @param  None
 * @retval None
 */
void displayScore() {
	char textScore[50];

	sprintf(textScore,"%d",score);

	glEnable(GL_COLOR_MATERIAL);

	glPushMatrix();
	glColor3f(10,10,10);
	glScalef(0.1,0.1,0.1);

	glTranslatef(10*PAC_Position.x + 12, 2, 10*PAC_Position.z - 12);
	glRotatef(-camAngle, 1, 0, 0);


	t3dDraw2D("Score", 0, 0, 0);

	glTranslatef(0, -1.5, 0);
	t3dDraw2D(textScore, 0, 0, 0);

	glPopMatrix();

	glDisable(GL_COLOR_MATERIAL);
}
