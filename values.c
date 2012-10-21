/*
 * values.c
 *
 *  Created on: 17 oct. 2012
 *      Author: blackswords
 */

#include "values.h"

/***				Pacman related values				***/
direction 	PAC_Direction 	= NONE;
point		PAC_Position 	= {0, 0, 0};
float		PAC_PosInc 		= 0.01;
point		PAC_Angle		= {0,0,0};

/***				General values						***/
point		GameBaseSize = {2.0f , 0,  2.217f};

int		speed = 30;
