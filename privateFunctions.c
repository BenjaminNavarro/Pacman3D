#include "privateFunctions.h"

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
				 0,                            //0 for now
				 GL_RGB,                       //Format OpenGL uses for image
				 image->width, image->height,  //Width and height
				 0,                            //The border of the image
				 GL_RGB, //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
				                   //as unsigned numbers
				 image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}

// Call glColor3f using RGB values (0-255)
void glColorRGB(unsigned char r, unsigned char g, unsigned char b) {
	GLfloat red, green, blue;
	red 	= (float) r / 255.0;
	green 	= (float) g / 255.0;
	blue 		= (float) b / 255.0;

	glColor3f(red, green, blue);
}

// Call glColor4f using RGB values (0-255) and alpha (0-1)
void glColorRGBAlpha(unsigned char r, unsigned char g, unsigned char b, float alpha) {
	GLfloat red, green, blue;
	red 	= (float) r / 255.0;
	green 	= (float) g / 255.0;
	blue 		= (float) b / 255.0;

	glColor4f(red, green, blue, alpha);
}
