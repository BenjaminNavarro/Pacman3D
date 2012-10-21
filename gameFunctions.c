/*
 * gameFunctions.c
 *
 *  Created on: 20 oct. 2012
 *      Author: blackswords
 */

#include "gameFunctions.h"

gridPosition locateOnGrid(point position) {
	gridPosition grid;

	point loc;

	loc.x = (position.x + GameBaseSize.x) / (2*GameBaseSize.x);		// Normalize to 0-1
	loc.x *= (float) N_SQUARES_W;									// Normalize to 0-N_SQUARES_W

	loc.z = (position.z + GameBaseSize.z) / (2*GameBaseSize.z);		// Normalize to 0-1
	loc.z *= (float) N_SQUARES_H;									// Normalize to 0-N_SQUARES_H

	grid.x = (unsigned int) floor(loc.x);
	grid.z = (unsigned int) floor(loc.z);

	return grid;
}

point gridToPos(gridPosition grid) {
	point position;

	position.x = (float) grid.x / (float) (N_SQUARES_W-1);
	position.z = (float) grid.z / (float) (N_SQUARES_H-1);

	position.x = -GameBaseSize.x + position.x * 2 * GameBaseSize.x;
	position.z = -GameBaseSize.z + position.z * 2 * GameBaseSize.z;

	return position;
}

bool onXCenter(point position) {
	point centerPos = gridToPos(locateOnGrid(position));

	if(absf(position.x - centerPos.x) <= PAC_PosInc*1.1f)
		return true;

	return false;
}

bool onZCenter(point position) {
	point centerPos = gridToPos(locateOnGrid(position));

	if(absf(position.z - centerPos.z) <= PAC_PosInc*1.1f)
		return true;

	return false;
}
