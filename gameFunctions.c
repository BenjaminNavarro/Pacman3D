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


void movePacman(direction dir) {
	static gridPosition	currentPosition;
	static gridPosition	nextPosition;
	static bool 		moving = false;
	static direction	currentDirection = NONE;

	// Locate Pacman on the grid
	currentPosition = locateOnGrid(PAC_Position);

	// Test if the next cell is a correct one
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

		if(nextPosition.x == 0) {
			PAC_Position.x = GameBaseSize.x;
			nextPosition.x = N_CELLS_W-2;
		}

		if(nextPosition.x == N_CELLS_W-1) {
			PAC_Position.x = -GameBaseSize.x;
			nextPosition.x = 1;
		}

		currentDirection = dir;
		moving = true;
	}
	else {
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

		if(onCellCenter(PAC_Position, nextPosition))
			moving = false;
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
