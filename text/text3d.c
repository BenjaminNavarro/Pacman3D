/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */



/* The 3D font is stored in an external file called "charset", which is
 * basically a compact way of representing a series of OpenGL commands for
 * drawing each of the printable ASCII characters, other than the space
 * character (33 to 126).  The file has the following format:
 * 
 * 
 * 
 * the characters "VTR\0FNT\0"
 * float space_width (the width of ' ', relative to the height the font)
 * 
 * float char_33_scale
 * unsigned short char_33_width (width = value * scale / 65536)
 * unsigned short char_33_height (height = value * scale / 65536)
 * unsigned short char_33_num_verts
 * unsigned short char_33_vert_1_x (x = value * scale / 65536 + scale / 2)
 * unsigned short char_33_vert_1_y (y = value * scale / 65536 + scale / 2)
 * unsigned short char_33_vert_2_x (x = value * scale / 65536 + scale / 2)
 * unsigned short char_33_vert_2_y (y = value * scale / 65536 + scale / 2)
 * ...
 * unsigned short char_33_vert_n_x (x = value * scale / 65536 + scale / 2)
 * unsigned short char_33_vert_n_y (y = value * scale / 65536 + scale / 2)
 * unsigned short opcode_1_for_char_33_front_face
 * unsigned short opcode_2_for_char_33_front_face
 * ...
 * unsigned short opcode_n_for_char_33_front_face
 * unsigned short end_part_opcode
 * unsigned short opcode_1_for_char_33_3D_part
 * unsigned short opcode_2_for_char_33_3D_part
 * ...
 * unsigned short opcode_n_for_char_33_3D_part
 * unsigned short end_part_opcode
 * 
 * float char_34_scale
 * ...
 * 
 * 
 * 
 * The character models are centered at (0, 0[, 0]).
 * 
 * unsigned shorts are represented in little-endian format.  floats are
 * represented using one signed character exp followed by one signed integer
 * mant, presented in little-endian format.  This represents the number
 * mant * 2^exp if mant is positive and -(~mant * 2^exp) if mant is negative.
 * 
 * The opcodes are as follows:
 * 
 * 0 to num_verts - 1:
 *     vertex with the same index as the opcode, using the vertex on the front
 *     face (the one with normal (0, 0, 1))
 * num_verts to 2 * num_verts - 1:
 *     Vertex with index opcode - num_verts, using the vertex on the back face
 *     (the one with normal (0, 0, -1)).  This opcode is only available for the
 *     3D part of each model.
 * 65532:
 *     Normal vector.  Followed by an unsigned short indicating the angle of the
 *     vector divided by (2 pi) times 65536.  The normal vector indicated is
 *     (cos theta, sin theta, 0).  This opcode is only available for the 3D part
 *     of each model.
 * 65533: GL_TRIANGLE_STRIP
 * 65534: GL_TRIANGLES
 * 65535: end_part
 * 
 * Vertices must be specified in counterclockwise order, or, in the case of
 * triangle strips, they must be specified such that the first three vertices
 * indicate a triangle in counterclockwise order.  When specifying the 2D part
 * of the model, counterclockwise order is relative to the front face.
 */



#include "text3d.h"

enum Opcodes {OP_NORMAL = 65532,
			  OP_TRIANGLE_STRIP,
			  OP_TRIANGLES,
			  OP_END_PART};

float PI_TIMES_2_OVER_65536 = 2 * 3.1415926535f / 65536.0f;


T3DLoadException error;


void T3DLoadException_init(T3DLoadException *p, char* message1) {

	p->message0 = strdup (message1);	

};
char* T3DLoadException_message(T3DLoadException *p) {

	return p->message0;
};



//Converts a four-character array to an integer, using little-endian form
extern int toInt(const char* bytes);	// in imageloader.c
	
//Converts a five-character array to a float, as indicated in the comment at
//the top of this file
float toFloat(const char* buffer) {
	char exp = buffer[0];
	int mant = toInt(buffer + 1);
	int isNegative;
	if (mant < 0) {
		isNegative = 1;
		mant = ~mant;
	}
	else
		isNegative = 0;
	float a = (2147483648u + (unsigned int)mant) *
				pow(2.0f, exp) / 2147483648.0;
	return isNegative ? -a : a;
}
	
//Converts a two-character array to an unsigned short, using little-endian
//form
unsigned short toUShort(const char* buffer) {
	return (((unsigned short)((unsigned char)buffer[1])) << 8) +
			(unsigned short)((unsigned char)buffer[0]);
}
	
	

void T3DFont_init (T3DFont *T3Dp, FILE* input) {
	char buffer[8];
	int i,j, e;


	e = fread(buffer, 1, 8, input);


	if (e == 0) {
		T3DLoadException_init(&error,"Invalid font file");
	}

	

	char header[9] = "VTR\0FNT\0";

	for(i  = 0; i < 8; i++) {
		if (buffer[i] != header[i]) {
			T3DLoadException_init(&error,"Invalid font file");
		}
	}
	
	fread(buffer, 1, 5, input);
	T3Dp->spaceWidth = toFloat(buffer);
	
	T3Dp->displayListId2D = glGenLists(94);
	T3Dp->displayListId3D = glGenLists(94);
	float* verts;
	float* verts2;
	

	for(i = 0; i < 94; i++) {
		fread(buffer, 1, 5, input);
		float scale = toFloat(buffer) / 65536;
		fread(buffer, 1, 2, input);
		float width = scale * toUShort(buffer);
		fread(buffer, 1, 2, input);
		float height = scale * toUShort(buffer);
		scale /= height;
		T3Dp->widths[i] = width / height;
		fread(buffer, 1, 2, input);

		unsigned short numVerts = toUShort(buffer);


		
		verts = (float*)malloc (sizeof (float)*2 * numVerts);	
		verts2 = (float*)malloc (sizeof (float)*2 * numVerts);	

		
		for(j = 0; j < numVerts; j++) {
			fread(buffer, 1, 2, input);
			verts2[2 * j] = scale * ((int)toUShort(buffer) - 32768);
			fread(buffer, 1, 2, input);
			verts2[2 * j + 1] =
				scale * ((int)toUShort(buffer) - 32768);
		}
		
		//Face part of the model
		glNewList(T3Dp->displayListId2D + i, GL_COMPILE);
		
		glNormal3f(0, 0, 1);
		
		fread(buffer, 1, 2, input);
		unsigned short opcode = toUShort(buffer);
		switch(opcode) {
			case OP_TRIANGLES:
				glBegin(GL_TRIANGLES);
				break;
			case OP_TRIANGLE_STRIP:
				glBegin(GL_TRIANGLE_STRIP);
				break;
			default:
				T3DLoadException_init(&error,"Invalid font file");
				break;
		}
		
		//Prevents excessive iteration or infinite loops on invalid
		//font files
		int limit = 10000;
		
		while(1) {
			fread(buffer, 1, 2, input);
			opcode = toUShort(buffer);
			switch(opcode) {
				case OP_TRIANGLES:
					glEnd();
					glBegin(GL_TRIANGLES);
					break;
				case OP_TRIANGLE_STRIP:
					glEnd();
					glBegin(GL_TRIANGLE_STRIP);
					break;
				case OP_END_PART:
					goto BreakOuter;
				default:
					glVertex3f(verts2[2 * opcode],
							   verts2[2 * opcode + 1],
							   0);
					break;
			}
			
			if (--limit == 0) {
				glEndList();
				T3DLoadException_init(&error,"Invalid font file");
			}
		}
BreakOuter:
		glEnd();
		glEndList();
		
		//3D part of the model
		glNewList(T3Dp->displayListId3D + i, GL_COMPILE);
		glPushMatrix();
		glTranslatef(0, 0, 0.5f);
		glFrontFace(GL_CW);
		glCallList(T3Dp->displayListId2D + i);
		glTranslatef(0, 0, -1);
		glScalef(1, 1, -1);
		glFrontFace(GL_CCW);
		glCallList(T3Dp->displayListId2D + i);
		glFrontFace(GL_CW);
		
		fread(buffer, 1, 2, input);
		opcode = toUShort(buffer);
		switch(opcode) {
			case OP_TRIANGLES:
				glBegin(GL_TRIANGLES);
				break;
			case OP_TRIANGLE_STRIP:
				glBegin(GL_TRIANGLE_STRIP);
				break;
			default:
				T3DLoadException_init(&error,"Invalid font file");
				break;
		}
		
		limit = 10000;
		while(1) {
			fread(buffer, 1, 2, input);
			opcode = toUShort(buffer);
			switch(opcode) {
				case OP_TRIANGLES:
					glEnd();
					glBegin(GL_TRIANGLES);
					break;
				case OP_TRIANGLE_STRIP:
					glEnd();
					glBegin(GL_TRIANGLE_STRIP);
					break;
				case OP_NORMAL:
					fread(buffer, 1, 2, input);

					float angle;
					angle = toUShort(buffer) *
						PI_TIMES_2_OVER_65536;
					float x, y;
					x = cos(angle);
					y = sin(angle);
					glNormal3f(x, y, 0);
					break;
				case OP_END_PART:
					goto BreakOuter2;
				default:
					if (opcode < numVerts) {
						glVertex3f(verts2[2 * opcode],
								   verts2[2 * opcode + 1],
								   0);
					}
					else {
						glVertex3f(verts2[2 * (opcode - numVerts)],
								   verts2[2 * (opcode - numVerts) +
										  1],
								   -1);
					}
					break;
			}
			
			if (--limit == 0) {
				glEndList();
				T3DLoadException_init(&error,"Invalid font file");
			}
		}
BreakOuter2:
		glEnd();
		glPopMatrix();
		glEndList();

		free (verts);
		free (verts2);

	}
	
	fread(buffer, 1, 2, input);
}
			
void T3DFont_draw2D(T3DFont *T3Dp,char c) {
	if (c >= 33 && c <= 126) {
		glCallList(T3Dp->displayListId2D + c - '!');
	}
}
			
void T3DFont_draw3D(T3DFont *T3Dp,char c) {
	if (c >= 33 && c <= 126) {
		glCallList(T3Dp->displayListId3D + c - '!');
	}
}
			
float T3DFont_width(T3DFont *T3Dp,char c) {
	if (c >= 33 && c <= 126) {
		return T3Dp->widths[c - 33];
	}
	else {
		return T3Dp->spaceWidth;
	}
}

	

T3DFont* font = NULL; //The font used to draw 2D and 3D characters

	
void draw2D(char c) {
	T3DFont_draw2D (font, c);
}
	
void draw3D(char c) {
	T3DFont_draw3D (font, c);
}
	

void drawLine(const char* str, int hAlign, void (*drawFunc)(char)) {
	int i;
	glPushMatrix();
	if (hAlign >= 0) {
		float width = 0;
		for( i = 0; str[i] != '\n' && str[i] != '\0'; i++) {
			width += T3DFont_width (font, str[i]);
		}
		glTranslatef(hAlign > 0 ? -width : -width / 2, 0, 0);
	}
	
	for(i = 0; str[i] != '\n' && str[i] != '\0'; i++) {
		float width = T3DFont_width (font, str[i]);
		glTranslatef(width / 2, 0, 0);
		drawFunc(str[i]);
		glTranslatef(width / 2, 0, 0);
	}
	
	glPopMatrix();
}
	
void draw(const char* str,
		  int hAlign, int vAlign,
		  float lineHeight,
		  void (*drawFunc)(char)) {

	int i;

	GLint shadeModel;
	glGetIntegerv(GL_SHADE_MODEL, &shadeModel);
	glShadeModel(GL_SMOOTH);
	GLboolean lightsEnabled;
	glGetBooleanv(GL_LIGHTING, &lightsEnabled);
	GLboolean normalsWereNormalized;
	glGetBooleanv(GL_NORMALIZE, &normalsWereNormalized);
	if (lightsEnabled) {
		glEnable(GL_NORMALIZE);
	}
	else {
		glDisable(GL_NORMALIZE);
	}
	
	glPushMatrix();
	if (vAlign >= 0) {
		int numLines = 1;
		for(i = 0; str[i] != '\0'; i++) {
			if (str[i] == '\n') {
				numLines++;
			}
		}
		
		float height = lineHeight * (numLines - 1) + 1;
		glTranslatef(0, vAlign > 0 ? height : height / 2, 0);
	}
	
	glTranslatef(0, -0.5f, 0);
	drawLine(str, hAlign, drawFunc);
	for(i = 0; str[i] != '\0'; i++) {
		if (str[i] == '\n') {
			glTranslatef(0, -lineHeight, 0);
			drawLine(str + i + 1, hAlign, drawFunc);
		}
	}
	
	glPopMatrix();
	
	glShadeModel(shadeModel);
	if (normalsWereNormalized) {
		glEnable(GL_NORMALIZE);
	}
	else {
		glDisable(GL_NORMALIZE);
	}
}


void t3dInit() {
	if (font == NULL) {

		font = (T3DFont*) malloc (sizeof (T3DFont) );

		FILE* input;
		input = fopen ("./text/charset", "rb");

		if(input == NULL) {
			printf("Unable to open the charset file\n");
			exit(0);
		}

		T3DFont_init (font, input);

		fclose(input);
	}
}

void t3dCleanup() {
	//free font;
}

void t3dDraw2D(char * str, int hAlign, int vAlign, float lineHeight) {
	GLboolean wasCulling;
	glGetBooleanv(GL_CULL_FACE, &wasCulling);
	glDisable(GL_CULL_FACE);
	
	draw(str, hAlign, vAlign, lineHeight, draw2D);
	
	if (wasCulling) {
		glEnable(GL_CULL_FACE);
	}
}

void t3dDraw3D(char *str,
			   int hAlign, int vAlign,
			   float depth,
			   float lineHeight) {
	GLboolean wasCulling;
	glGetBooleanv(GL_CULL_FACE, &wasCulling);
	glEnable(GL_CULL_FACE);
	GLint frontFace;
	glGetIntegerv(GL_FRONT_FACE, &frontFace);
	
	glPushMatrix();
	glScalef(1, 1, depth);
	draw(str, hAlign, vAlign, lineHeight, draw3D);
	glPopMatrix();
	
	if (!wasCulling) {
		glDisable(GL_CULL_FACE);
	}
	glFrontFace(frontFace);
}

float t3dDrawWidth(char *str) {
	float bestWidth = 0;
	int i = 0;
	printf ("begin t3dDrawWidth \n");
	while (str[i] != '\0') {
		float width = 0;
		while (str[i] != '\n' && str[i] != '\0') {
			width += T3DFont_width (font, str[i]);
			i++;
		}
		if (width > bestWidth) {
			bestWidth = width;
		}
		if (str[i] != '\0') {
			i++;
		}
	}
	printf ("end  t3dDrawWidth %f \n",bestWidth);
	return bestWidth;
}

float t3dDrawHeight(char *str, float lineHeight) {
	int numLines = 1;
	int i;
	for(i = 0; str[i] != '\0'; i++) {
		if (str[i] == '\n') {
			numLines++;
		}
	}
	
	return (numLines - 1) * lineHeight + 1;
}









