#ifndef _OPENGLFUNCTIONS_
#define _OPENGLFUNCTIONS_

/***				Includes					***/
// Standard libraries
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// OpenGL / GLUT libraries
#include "OpenGLHeader.h"

#include "imageloader.h"
#include "objImport/glm.h"
#include "objImport/gltb.h"
#include "objImport/dirent32.h"

// Personnal libraries
#include "privateFunctions.h"
#include "values.h"
#include "gameFunctions.h"

/***				Defines						***/
#define SHOW_GRID		0

/***				Prototypes					***/
void drawScene();
void loadModels();
void handleKeypress(unsigned char key, int x, int y);
void handleKeyup(unsigned char key, int x, int y);
void initRendering();
void handleResize(int w, int h);
void PAC_Update(int value);


#endif
