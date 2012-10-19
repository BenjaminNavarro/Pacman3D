#ifndef _OPENGLFUNCTIONS_
#define _OPENGLFUNCTIONS_

// Standard libraries
#include <stdlib.h>
#include <stdio.h>

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
#include "objImport/glm.h"
#include "objImport/gltb.h"
#include "objImport/dirent32.h"

// Personnal libraries
#include "privateFunctions.h"
#include "values.h"

/***				Prototypes					***/
void drawScene();
void handleKeypress(unsigned char key, int x, int y);
void handleKeyup(unsigned char key, int x, int y);
void initRendering();
void handleResize(int w, int h);
void PAC_Update(int value);


#endif
