#ifndef _PRIVATEFUNCTION_
#define _PRIVATEFUNCTION_

/***				includes				***/
// Standard libraries
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// OpenGL / GLUT libraries
#include "OpenGLHeader.h"

#include "images/imageloader.h"
#include "values.h"

/***				Prototypes				***/
GLuint 	loadTexture(Image* image);
void	glColorRGB(unsigned char r, unsigned char g, unsigned char b);
void 	glColorRGBAlpha(unsigned char r, unsigned char g, unsigned char b, float alpha);
float	degToRad(float angle);

void assert_failed(unsigned char* file, unsigned int line);

/***				Defines					***/
#define	FAILED	0
#define SUCCESS	1

#define assert(expr) 				((expr) ? (void)0 : assert_failed((unsigned char *)__FILE__, __LINE__))

#define isValidPosition(pos)		((pos.x >= -GameBaseSize.x) && \
									 (pos.x <=  GameBaseSize.x) && \
									 (pos.z >= -GameBaseSize.z) && \
									 (pos.z <=  GameBaseSize.z))

#define isValidCell(cell)			((cell.x <= N_CELLS_W) && \
									 (cell.z <= N_CELLS_H))

#define isSameCell(cell1,cell2)		(((cell1.x) == (cell2.x)) && \
									 ((cell1.z) == (cell2.z)))

#define isOnWall(cell)				(GameBoard[cell.z][cell.x] != WALL)
#define wallOnTop(cell)				(GameBoard[cell.z-1][cell.x] == WALL)
#define wallOnBottom(cell)			(GameBoard[cell.z+1][cell.x] == WALL)
#define wallOnLeft(cell)			(GameBoard[cell.z][cell.x-1] == WALL)
#define wallOnRight(cell)			(GameBoard[cell.z][cell.x+1] == WALL)



#endif
