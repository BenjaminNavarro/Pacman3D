#ifndef _PRIVATEFUNCTION_
#define _PRIVATEFUNCTION_

/***				includes				***/
// Standard libraries
#include <stdlib.h>

// OpenGL / GLUT libraries
#include <OpenGL.h>
#include <glut.h>
#include <glu.h>
#include <gl.h>

#include "imageloader.h"

/***				Prototypes				***/
GLuint 	loadTexture(Image* image);
void	glColorRGB(unsigned char r, unsigned char g, unsigned char b);
void 	glColorRGBAlpha(unsigned char r, unsigned char g, unsigned char b, float alpha);

#endif
