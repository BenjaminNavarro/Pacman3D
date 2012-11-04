/*
 * gameFunctions.h
 *
 *  Created on: 20 oct. 2012
 *      Author: blackswords
 */

#ifndef GAMEFUNCTIONS_H_
#define GAMEFUNCTIONS_H_

/***				Includes					***/
// Standard libraries
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// OpenGL / GLUT libraries
#include "OpenGLHeader.h"

// Personnal libraries
#include "OpenGLFunctions.h"
#include "privateFunctions.h"
#include "text/text3d.h"
#include "values.h"

/***				Variable					***/

/***				Prototypes					***/
gridPosition 	locateOnGrid(point position);
point			gridToPos(gridPosition grid);
bool			onCellCenter(point position, gridPosition cell);
void			movePacman(direction dir);
void			moveGhosts();
void			renderGame();
void			checkCellAction(gridPosition grid);
void			displayHUD();
gridPosition	getNextPosition();
void			resetPositions();
void			gameOver();
void 			sendPacmanHome();
void			sendGhostHome(int ghostNum);
void			sendGhostBoard(int ghostNum);
void			startCombo(int value);
void 			checkGhosts();
void			gameBoardInit();
void			levelUp();

#endif /* GAMEFUNCTIONS_H_ */
