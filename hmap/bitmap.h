#ifndef BITMAP_CLASS_H
#define BITMAP_CLASS_H

#include <stdlib.h>
#include <windows.h>
#include "udt.h"
using namespace std;

/*+----------------------------------------------------------+
| cBitmap |
+----------------------------------------------------------+
| |
| Purpose: |
| The cBitmap class makes it easy to load and use bitmaps.|
| |
+----------------------------------------------------------+*/

class cBitmap
{
public:
	// constructor 
	cBitmap(char * filename, 
	BYTE red_color_key, 
	BYTE green_color_key, 
	BYTE blue_color_key);

	// arrays are dynamic, so we can't have any leaks
	~cBitmap();

	// returns the width of the bitmap in pixels
	LONG GetWidth();

	// returns the height of the bitmap in pixels
	LONG GetHeight();

	// returns a long int with rgba data for point (x, y)
	COLORREF GetPixel(int x, int y);

	// paints a pixel in the bitmap
	void PutPixel(int x, int y, 
	BYTE red, BYTE green, BYTE blue, BYTE alpha);

	// saves a bitmap to the given file
	void SaveBitmap(char * filename);

private:
	// bitmap file header, standard Winblows format
	BITMAPFILEHEADER m_header;

	// bitmap information, like height, width, and pixel format
	BITMAPINFOHEADER m_info;

	// pixel data
	BYTE * m_pixel_data;
	BYTE * m_rgba_data;

	// used for transparency (m_rgba_data is OpenGL ready)
	color_vector m_colorkey;
};

#endif