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

typedef point vect3;

typedef struct {
	float x1, x2;
	float y1, y2;
	float z1, z2;
} line;

typedef struct {
	unsigned int x;
	unsigned int z;
} gridPosition;

typedef enum {
	NONE		=0,
	FORWARD		=1,
	BACKWARD	=2,
	LEFT		=4,
	RIGHT		=8
} direction;

typedef enum {
	EMPTY,
	COIN,
	BIGCOIN,
	FRUIT,
	WALL,
	GHOSTOUT
} cellType;

typedef unsigned char bool;

enum {
	false = 0,
	true
};

#define 	setVect3(vec, xv , yv , zv) 	vec.x = xv; vec.y = yv; vec.z = zv;

/***				Pacman related values				***/
#define 	PAC_RADIUS		0.07

#define		PAC_ANIM_FRAMES	10

extern 		direction 		PAC_Direction;		// The direction where pacman should go
extern 		point			PAC_Position;		// The position of pacman in the level
extern 		float 			PAC_PosInc;			// At each timer update, pacman moves to PAC_PosInc in PAC_Direction
extern		point			PAC_Angle;			// The orientation of pacman

/***				Ghosts related values				***/
#define		GHOST_SCALE		0.10
#define 	GHOST_COUNT		1

extern		direction		Ghost_Direction[GHOST_COUNT];
extern		point			Ghost_Position[GHOST_COUNT];
extern		float			Ghost_PosInc[GHOST_COUNT];
extern		point			Ghost_Angle[GHOST_COUNT];


/***				General values						***/
extern		point			GameBaseSize;
extern		int				speed;				// Pacman timer update period in milliseconds
extern		int				animationSpeed;		// Pacman animation timer update period in milliseconds
extern		int				ghostSpeed;			// Ghosts timer update period in milliseconds
extern		int				score;
extern		int				level;

#define		COIN_POINTS		10
#define		BIGCOIN_POINTS	50

#define		N_CELLS_W		28
#define		N_CELLS_H		31

#define		OBJECTS_HEIGHT	0.06
#define		COIN_RADIUS		0.01
#define		BIGCOIN_RADIUS	0.05

extern		cellType		GameBoard[N_CELLS_H][N_CELLS_W];

#endif /* VALUES_H_ */
