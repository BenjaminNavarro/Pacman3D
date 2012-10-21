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
#include "values.h"

/***				Defines						***/


/***				Prototypes					***/
gridPosition 	locateOnGrid(point position);
point			gridToPos(gridPosition grid);
bool			onXCenter(point position);
bool			onZCenter(point position);

#endif /* GAMEFUNCTIONS_H_ */
