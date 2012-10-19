/*	
 *	Pacman3D
 *
 *	Benjamin Navarro
 *	M1 EEA- Université Montpellier 2
 *	2012 - 2013
 *
*/

/***				Includes					***/
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


// Personnal libraries
#include "OpenGLFunctions.h"
#include "values.h"


int main(int argc, char** argv) {
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	
	//Create the window
	glutCreateWindow("Pacman");
	initRendering();
	
	//Set handler functions
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutKeyboardUpFunc(handleKeyup);
	glutReshapeFunc(handleResize);
	
	glutTimerFunc(speed, PAC_Update, 0); //Add a timer
	
	glutMainLoop();
	return 0;
}





