/*
 * values.h
 *
 *  Created on: 17 oct. 2012
 *      Author: blackswords
 */

#ifndef VALUES_H_
#define VALUES_H_

/***				types & structs			***/
typedef struct {
	float x;
	float y;
	float z;
} point;

typedef struct {
	float x1, x2;
	float y1, y2;
	float z1, z2;
} line;

typedef enum {
	NONE		=0,
	FORWARD		=1,
	BACKWARD	=2,
	LEFT		=4,
	RIGHT		=8
} direction;

/***				Pacman related values				***/
#define 	PAC_RADIUS		0.2

extern 		direction 		PAC_Direction;		// The direction where pacman should go
extern 		point			PAC_Position;		// The position of pacman in the level
extern 		float 			PAC_PosInc;			// At each timer update, pacman moves to PAC_PosInc in PAC_Direction


/***				General values						***/
#define 	GAME_BASE_SIZE	2.0

extern		int			speed;					// timers update period in milliseconds

#endif /* VALUES_H_ */
