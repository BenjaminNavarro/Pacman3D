#ifndef _PRIVATEFUNCTION_
#define _PRIVATEFUNCTION_

/***				includes				***/
// Standard libraries
#include <stdlib.h>

// OpenGL / GLUT libraries
#ifdef __APPLE__
	#include <OpenGL.h>
	#include <glut.h>
	#include <glu.h>
	#include <gl.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
#endif

#include "imageloader.h"

/***				Prototypes				***/
GLuint 	loadTexture(Image* image);
void	glColorRGB(unsigned char r, unsigned char g, unsigned char b);
void 	glColorRGBAlpha(unsigned char r, unsigned char g, unsigned char b, float alpha);

#endif
