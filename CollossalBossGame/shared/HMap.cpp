#include <stdlib.h>
#include <windows.h>
#include "HMap.h"
#include "defs.h"
#include "DebugConsole.h"
#include <fstream>
#include <cfloat>
using namespace std;

/*
 * Constructs a heightmap from the specified grayscale bitmap.
 *   filename   - The name of the bitmap file.
 *   unitLength - The length of one unit (how much world-length one bmp pixel 
 *				  represents)
 *   scale      - Factor for scaling heights; from 0 -> 255 to 0 -> 255 * scale
 */
HMap::HMap(const char * filename, int unitLength, float scale) {
	//Input stream for reading in the bitmap
	ifstream fin;

	//Create header structures
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER info;

	//Initialize pointers, and create a place for pixel data to be stored
	BYTE * pixelData = NULL;
	_hdata = 0;

	// attempt to open the file using binary access
	fin.open(filename, ios::binary);

	if(fin.is_open()) {
		// read in the file info 
		fin.read((char *)(&header), sizeof(BITMAPFILEHEADER));
		fin.read((char *)(&info), sizeof(BITMAPINFOHEADER));
		_w = info.biWidth;
		_l = info.biHeight;
		_ul = unitLength;

		// create an array that can take the pixel data
		pixelData = new BYTE[info.biSizeImage];

		//We want to store the actual heights
		_hdata = new float[_w * _l];

		// read the pixels
		fin.read((char *)(pixelData), info.biSizeImage);

		// close the file
		fin.close();

		// now, the hard part - we need to get the BGR data to RGBA
		BYTE * src(pixelData);
		float * dst(_hdata);

		//Pixel rows are padded to a full set of words
		int paddingSize = _w % 4;
		BYTE maxByte = 0,
			 minByte = 0xff,
			 curByte = 0;

		//Convert 24-bit rgb pixels to floating-point heights
		for(int index(0); index <  _w * _l; index++) {
			// Convert the average rgb value from 0-255 to an arbitrary floating-point scale
			curByte = ((*(src + 2)) + (*(src + 1)) + (*(src + 0))) / 3.0f;
			*(dst) = scale * curByte;

			if(curByte > maxByte) {
				maxByte = curByte;
			}
			if(curByte < minByte) {
				minByte = curByte;
			}

			//Skip padding bytes
			if((index) % _w == _w - 1) {
				src += paddingSize;
			}

			//get the next element in each array
			src += 3;
			dst++;
		}

		_max = scale * maxByte;
		_min = scale * minByte;

		delete [] pixelData;
	} else {
		//File not found
		DC::get()->print("ERROR: Heightmap file %s not found\n", filename);
	}
}


HMap::~HMap(void) {
	if(_hdata) {
		delete [] _hdata;
	}
}


float HMap::getHeightAt(int i, int j) {
	return _hdata[i + j * _w];
}

