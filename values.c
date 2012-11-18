/*
 * values.c
 *
 *  Created on: 17 oct. 2012
 *      Author: blackswords
 */

#include "values.h"

/***				Pacman related values				***/
direction 	PAC_Direction 	= NONE;
point		PAC_Position 	= {0,	 0,	 0};
float		PAC_PosInc 		= 0.01;

/***				Ghost related values				***/
direction		Ghost_Direction[GHOST_COUNT];
point			Ghost_Position[GHOST_COUNT];
float			Ghost_PosInc = 0.01;

/***				General values						***/
point			GameBaseSize 		= {2.0f ,	 0,	  2.217f};
int				speed 				= 10;
int				animationSpeed		= 25;
int				ghostSpeed			= 15;
int				score				= 0;
int				level				= 2;
bool			hunted				= true;
int				lives				= MAX_LIVES;
int				comboValue			= 200;
int				comboTimeRemaining	= 0;
bool			ghostInit			= true;
int				coinsLeft			= 0;
int				coinsPicked			= 0;
gridPosition	fruitPosition		= {15,	17};

cellType 	GameBoard[N_CELLS_H][N_CELLS_W];
cellType 	GameBoardInit[N_CELLS_H][N_CELLS_W] = {
/*0*/	{WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL},
/*1*/	{WALL,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	WALL,	WALL,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	WALL},
/*2*/	{WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL},
/*3*/	{WALL,	BIGCOIN,WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	BIGCOIN,WALL},
/*4*/	{WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL},
/*5*/	{WALL,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	WALL},
/*6*/	{WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL},
/*7*/	{WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL},
/*8*/	{WALL,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	WALL,	WALL,	COIN,	COIN,	COIN,	COIN,	WALL,	WALL,	COIN,	COIN,	COIN,	COIN,	WALL,	WALL,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	WALL},
/*9*/	{WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	EMPTY,	WALL,	WALL,	EMPTY,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL},
/*10*/	{EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	EMPTY,	WALL,	WALL,	EMPTY,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY},
/*11*/	{EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	WALL,	COIN,	WALL,	WALL,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	WALL,	WALL,	COIN,	WALL,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY},
/*12*/	{EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	WALL,	COIN,	WALL,	WALL,	EMPTY,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	EMPTY,	WALL,	WALL,	COIN,	WALL,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY},
/*13*/	{WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	EMPTY,	WALL,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	WALL,	EMPTY,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL},
/*14*/	{EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	COIN,	EMPTY,	EMPTY,	EMPTY,	WALL,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	WALL,	EMPTY,	EMPTY,	EMPTY,	COIN,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY},
/*15*/	{WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	EMPTY,	WALL,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	WALL,	EMPTY,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL},
/*16*/	{EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	WALL,	COIN,	WALL,	WALL,	EMPTY,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	EMPTY,	WALL,	WALL,	COIN,	WALL,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY},
/*17*/	{EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	WALL,	COIN,	WALL,	WALL,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	WALL,	WALL,	COIN,	WALL,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY},
/*18*/	{EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	WALL,	COIN,	WALL,	WALL,	EMPTY,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	EMPTY,	WALL,	WALL,	COIN,	WALL,	EMPTY,	EMPTY,	EMPTY,	EMPTY,	EMPTY},
/*19*/	{WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	EMPTY,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	EMPTY,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL},
/*20*/	{WALL,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	WALL,	WALL,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	WALL},
/*21*/	{WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL},
/*22*/	{WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL},
/*23*/	{WALL,	BIGCOIN,COIN,	COIN,	WALL,	WALL,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	EMPTY,	EMPTY,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	WALL,	WALL,	COIN,	COIN,	BIGCOIN,WALL},
/*24*/	{WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL},
/*25*/	{WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL},
/*26*/	{WALL,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	WALL,	WALL,	COIN,	COIN,	COIN,	COIN,	WALL,	WALL,	COIN,	COIN,	COIN,	COIN,	WALL,	WALL,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	WALL},
/*27*/	{WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL},
/*28*/	{WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL,	WALL,	COIN,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	COIN,	WALL},
/*29*/	{WALL,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	COIN,	WALL},
/*30*/	{WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL,	WALL},
};
