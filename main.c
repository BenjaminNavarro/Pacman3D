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
#include <OpenGL.h>
#include <glut.h>
#include <glu.h>
#include <gl.h>

// Personnal libraries
#include "privateFunctions.h"
#include "OpenGLFunctions.h"


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
	glutReshapeFunc(handleResize);
	
	glutTimerFunc(25, update, 0); //Add a timer
	
	glutMainLoop();
	return 0;
}





