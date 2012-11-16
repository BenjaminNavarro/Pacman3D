#ifndef _PRIVATEFUNCTION_
#define _PRIVATEFUNCTION_

/***				includes				***/
// Standard libraries
#include <stdlib.h>
#include <math.h>

// OpenGL / GLUT libraries
#include "OpenGLHeader.h"

#include "images/imageloader.h"

/***				Prototypes				***/
GLuint 	loadTexture(Image* image);
void	glColorRGB(unsigned char r, unsigned char g, unsigned char b);
void 	glColorRGBAlpha(unsigned char r, unsigned char g, unsigned char b, float alpha);
float	degToRad(float angle);

#endif
