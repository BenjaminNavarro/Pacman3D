#ifndef _OPENGLFUNCTIONS_
#define _OPENGLFUNCTIONS_

// Standard libraries
#include <stdlib.h>

// OpenGL / GLUT libraries
#include <OpenGL.h>
#include <glut.h>
#include <glu.h>
#include <gl.h>

#include "imageloader.h"

// Personnal libraries
#include "privateFunctions.h"

/***				Prototypes					***/
void drawScene();
void handleKeypress(unsigned char key, int x, int y);
void initRendering();
void handleResize(int w, int h);
void update(int value);


#endif
