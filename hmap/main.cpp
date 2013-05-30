#include <iostream>
#include <windows.h>
#include "bitmap.h"
using namespace std;

// functions for the example
BYTE Red(COLORREF color);
BYTE Green(COLORREF color);
BYTE Blue(COLORREF color);

char densities[5] = {' ', (char)176, (char)177, (char)178, (char)219};

int main(int argc, char * argv)
{
	// some variables
	int red(0), green(0), blue(0);
	int x(0), y(0);
	COLORREF color(0);

	// open an existing bitmap (use your favorite here)
	cBitmap test("test.bmp", 0, 0, 0);

	// width and height 
	cout << "Width = " << test.GetWidth() << endl;
	cout << "Height = " << test.GetHeight() << endl;

	for(int i = 0; i < 5; ++i) {
		cout << densities[i];
	}
	cout << ";" << endl;

	// RGB data analysis
	for(int y = 0; y < test.GetHeight(); ++y) {
		for(int x = 0; x < test.GetWidth(); ++x) {
			// get a color in Windows format
			color = test.GetPixel(x, y);
			int r = Red(color);
			cout << densities[4 * r / 255];
			//cout << "At the point (" << x << ", " << y << ") the color is: (0x" << hex << color << ") " << dec;
			//cout << "(" << int(Red(color)) << ", " << int(Green(color)) << ", " << int(Blue(color)) << ")" << endl;
		}
		cout << ";" << endl;
	}

	// change a color "magenta at (0, 0)"
	test.PutPixel(0, 0, 255, 0, 255, 0);
	cout << "Placed a new pixel color at (0, 0)." << endl;

	// save the bitmap
	test.SaveBitmap("newfile.bmp");
	cout << "Saved the file to this directory as newfile.bmp." << endl;

	// destructor fired and deallocated memory
	cout << "Look in the top left corner of newfile.bmp!" << endl;

	cin >> x;
	return 0;
}

// function to obtain red data from a COLORREF value
BYTE Red(COLORREF color)
{
	return (color & 0x000000ff);
}

// and green
BYTE Green(COLORREF color)
{
	return (color & 0x0000ff00) >> 8;
}

// and also blue
BYTE Blue(COLORREF color)
{
	return (color & 0x00ff0000) >> 16;
}

// and alpha if you're froggy
BYTE Alpha(COLORREF color)
{
	return (color & 0xff000000) >> 24;
}