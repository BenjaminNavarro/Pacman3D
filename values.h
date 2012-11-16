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

typedef enum {
	Cherry,
	Strawberry,
	Orange,
	Apple,
	Grape,
	IceCompot,
	StewedFruit,
	Key
} fruits;

typedef struct move {
	gridPosition position;
	direction dir;
	struct move* next;
} move;

typedef struct {
	int numMoves;
	move* moves;
} lastMoves;

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

/***				Ghosts related values				***/
#define		GHOST_SCALE		0.10
#define 	GHOST_COUNT		4

extern		direction		Ghost_Direction[GHOST_COUNT];
extern		point			Ghost_Position[GHOST_COUNT];
extern		float			Ghost_PosInc;


/***				General values						***/
extern		point			GameBaseSize;
extern		int				speed;				// Pacman timer update period in milliseconds
extern		int				animationSpeed;		// Pacman animation timer update period in milliseconds
extern		int				ghostSpeed;			// Ghosts timer update period in milliseconds
extern		int				score;				// Store the player score
extern		int				level;				// Store the level number
extern		bool			hunted;				// True if Pacman is hunted by the ghosts, false otherwise
extern		int				lives;				// Number of lives remaining
extern		int				comboValue;			// Holds the last combo value
extern		int				comboTimeRemaining;	// Seconds to the end of combo period
extern		bool			ghostInit;			// True if the ghosts are sent to home for the first time
extern		int				coinsLeft;			// Holds the number of remaining coins to catch
extern 		int				coinsPicked;		// Holds the number of picked up coins
extern 		gridPosition	fruitPosition;		// Stores where the fruits appear

#define		MAX_LIVES		3
#define		COMBO_TIME		8

#define 	SCREEN_WIDTH	800
#define		SCREEN_HEIGHT	600

#define		COIN_POINTS		10
#define		BIGCOIN_POINTS	50

#define		N_CELLS_W		28
#define		N_CELLS_H		31

#define		OBJECTS_HEIGHT	0.06
#define		COIN_RADIUS		0.01
#define		BIGCOIN_RADIUS	0.05

#define		isFruitCoin(x)	(x == 70 ? true : x == 170 ? true : false)
#define		FRUIT_COUNT		8

extern		cellType		GameBoardInit[N_CELLS_H][N_CELLS_W];
extern		cellType		GameBoard[N_CELLS_H][N_CELLS_W];

#endif /* VALUES_H_ */
