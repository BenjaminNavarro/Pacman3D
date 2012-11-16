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



#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "imageloader.h"
	//Converts a four-character array to an integer, using little-endian form
int toInt(const unsigned char* bytes) {
	return (int)(((unsigned char)bytes[3] << 24) |
				 ((unsigned char)bytes[2] << 16) |
				 ((unsigned char)bytes[1] << 8) |
				 (unsigned char)bytes[0]);
}
	
	//Converts a two-character array to a short, using little-endian form
short toShort(const unsigned char* bytes) {
	return (short)(((unsigned char)bytes[1] << 8) |
				   (unsigned char)bytes[0]);
}
	
	//Reads the next four bytes as an integer, using little-endian form
int readInt(FILE *input) {
	unsigned char buffer[4];
	fread(buffer, 1, 4, input);
	return toInt(buffer);
}
	
	//Reads the next two bytes as a short, using little-endian form
short readShort(FILE *input) {
	unsigned char buffer[2];
	fread(buffer, 1, 2, input);
	return toShort(buffer);
}
	


Image* loadBMP(const char* filename) {
	FILE *input;
	int x,y,c;
	unsigned char buffer[2];
	unsigned char trash[8];
	Image *im_tmp;
	input = fopen (filename, "rb");
	if (input == NULL ) {
		return NULL;
	}
	
	fread(buffer, 1, 2, input);

	assert( (buffer[0] == 'B' && buffer[1] == 'M') || !"Not a bitmap file");

	fread(trash, 1, 8, input);

	int dataOffset = readInt(input);
	
	//Read the header
	int headerSize = readInt(input);
	int width;
	int height;
	switch(headerSize) {
		case 40:
			//V3
			width = readInt(input);
			height = readInt(input);
			fread(trash, 1, 2, input);


			assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			assert(readShort(input) == 0 || !"Image is compressed");


			break;
		case 12:
			//OS/2 V1
			width = readShort(input);
			height = readShort(input);
			fread(trash, 1, 2, input);
			assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			break;
		case 64:
			//OS/2 V2
			assert(!"Can't load OS/2 V2 bitmaps");
			break;
		case 108:
			//Windows V4
			assert(!"Can't load Windows V4 bitmaps");
			break;
		case 124:
			//Windows V5
			assert(!"Can't load Windows V5 bitmaps");
			break;
		default:
			assert(!"Unknown bitmap format");
			break;
	}
	
	//Read the data
	int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
	int size = bytesPerRow * height;


//	auto_array<char> pixels(new char[size]);
	unsigned char* pixels;
	pixels = (unsigned char*)malloc (sizeof (unsigned char)*size);	
	
	fseek (input,dataOffset, SEEK_SET );
//	input.seekg(dataOffset, ios_base::beg);
	

	fread(pixels, 1, size, input);
//	input.read(pixels.get(), size);
	
	//Get the data into the right format
//	auto_array<char> pixels2(new char[width * height * 3]);

	unsigned char* pixels2;
	pixels2 = (unsigned char*)malloc (sizeof (unsigned char)*width * height * 3);	
	

	for( y = 0; y < height; y++) {
		for( x = 0; x < width; x++) {
			for( c = 0; c < 3; c++) {
				pixels2[3 * (width * y + x) + c] =
					pixels[bytesPerRow * y + 3 * x + (2 - c)];
			}
		}
	}
	
//	input.close();
	fclose (input);

	im_tmp = (Image*) malloc (sizeof (Image));

	im_tmp->pixels = pixels2;
	im_tmp->width = width;
	im_tmp->height = height;
	
	return im_tmp;
}









