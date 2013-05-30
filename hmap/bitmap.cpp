#include <iostream>
#include <fstream>
#include <windows.h>
#include <iomanip>
#include "udt.h"
#include "bitmap.h"
using namespace std;

/*+----------------------------------------------------------+
| cBitmap |
+----------------------------------------------------------+*/
cBitmap::cBitmap(char * filename, 
	BYTE red_color_key, 
	BYTE green_color_key,
	BYTE blue_color_key)
{
	// need an input stream variable for file operations
	ifstream fin;

	// it is impolite to dangle your pointers
	m_pixel_data = 0;
	m_rgba_data = 0;

	// init the colorkey struct
	m_colorkey.r = red_color_key;
	m_colorkey.g = green_color_key;
	m_colorkey.b = blue_color_key;

	// attempt to open the file using binary access
	fin.open(filename, ios::binary);

	if(fin.is_open())
	{
		// read in the file info 
		fin.read((char *)(&m_header), sizeof(BITMAPFILEHEADER));
		fin.read((char *)(&m_info), sizeof(BITMAPINFOHEADER));

		// create an array that can take the pixel data
		m_pixel_data = new BYTE[m_info.biSizeImage];

		// rgba format requires 32 bits, not 24 (hence the * 4 / 3)
		m_rgba_data = new BYTE[m_info.biWidth * m_info.biHeight * 4];

		// read the pixels
		fin.read((char *)(m_pixel_data), m_info.biSizeImage);

		// close the file
		fin.close();

		// now, the hard part - we need to get the BGR data to RGBA
		BYTE * src(m_pixel_data);
		BYTE * dst(m_rgba_data);

		int paddingSize = m_info.biWidth % 4;

		// now we walk through the arrays and perform the operation
		for(unsigned int index(0); index <  m_info.biWidth * m_info.biHeight; index++)
		{
			// transform BGR to RGB (OpenGL uses this format - 
			// you can thank me later...)
			*(dst) = *(src + 2);
			*(dst + 1) = *(src + 1);
			*(dst + 2) = *(src);

			// now check the colorkey, and set colorkey 
			// pixels to alpha = 0
			if((*(dst) == red_color_key) && 
					(*(dst + 1) == green_color_key) &&
					(*(dst + 2) == blue_color_key)) {
				// colorkey matches, set to transparent
				*(dst + 3) = 0x00;
			} else {
				// the pixel isn't a colorkey pixel, set to opaque
				*(dst + 3) = 0xff;
			}

			//Skip padding
			if((index) % m_info.biWidth == m_info.biWidth - 1) {
				//for(int i = 0; i < paddingSize; ++i) { ++src; }
				src += paddingSize;
			}

			// increment the array pointers according
			src += 3;
			dst += 4;
		}
	}
	else
	{
		// post file not found message
		cout << "What is this " << filename << " you speak of?";
	}
	ofstream fout("out.txt");

	for(int y = m_info.biHeight - 1; y >= 0; --y) {
		for(int x = 0; x < m_info.biWidth; ++x) {
			
			int r = (int)m_rgba_data[(x + y * m_info.biWidth) * 4 + 0],
				g = (int)m_rgba_data[(x + y * m_info.biWidth) * 4 + 1],
				b = (int)m_rgba_data[(x + y * m_info.biWidth) * 4 + 2],
				a = (int)m_rgba_data[(x + y * m_info.biWidth) * 4 + 3];
			fout << hex << "(" << setw(2) << r << "," << setw(2) << g << "," << setw(2) << b << ":" << setw(2) << a << ") ";
		}
		fout << ";\n" << dec;
	}
}

/*+----------------------------------------------------------+
| ~cBitmap |
+----------------------------------------------------------+*/
cBitmap::~cBitmap()
{
	// deallocate arrays, if they were successfully allocated
	if(m_pixel_data) {
		delete [] m_pixel_data;
	}
	if(m_rgba_data) {
		delete [] m_rgba_data;
	}
}

/*+----------------------------------------------------------+
| GetWidth |
+----------------------------------------------------------+*/
LONG cBitmap::GetWidth() {
	return m_info.biWidth;
}

/*+----------------------------------------------------------+
| GetHeight |
+----------------------------------------------------------+*/
LONG cBitmap::GetHeight() {
	return m_info.biHeight;
}

/*+----------------------------------------------------------+
| GetPixel |
+----------------------------------------------------------+*/
COLORREF cBitmap::GetPixel(int x, int y) {
	// offset into the bitmap and place into a COLORREF variable
	COLORREF temp_color(0);
	y = m_info.biHeight - 1 - y;

	// grab the data from the array
	int r = (int)m_rgba_data[(x + y * m_info.biWidth) * 4 + 0],
		g = (int)m_rgba_data[(x + y * m_info.biWidth) * 4 + 1],
		b = (int)m_rgba_data[(x + y * m_info.biWidth) * 4 + 2],
		a = (int)m_rgba_data[(x + y * m_info.biWidth) * 4 + 3];

	// we can put the individual colors in the right format
	temp_color += a << 24;
	temp_color += b << 16;
	temp_color += g << 8;
	temp_color += r;

	return temp_color;
}

/*+----------------------------------------------------------+
| PutPixel |
+----------------------------------------------------------+*/
void cBitmap::PutPixel(int x, int y, BYTE red, BYTE green, BYTE blue, BYTE alpha)
{
	// offset into the bitmap and insert rgba vals
	int temp_red(0), temp_green(0), temp_blue(0), temp_alpha(0);
	int temp_x(x), temp_y(m_info.biHeight - 1 - y);

	// insert into the rgba array 
	m_rgba_data[4 * (temp_x + temp_y * m_info.biWidth)] = red;
	m_rgba_data[4 * (temp_x + temp_y * m_info.biWidth) + 1] = green;
	m_rgba_data[4 * (temp_x + temp_y * m_info.biWidth) + 2] = blue;
	m_rgba_data[4 * (temp_x + temp_y * m_info.biWidth) + 3] = alpha;
}

/*+----------------------------------------------------------+
| SaveBitmap |
+----------------------------------------------------------+*/
void cBitmap::SaveBitmap(char * filename)
{
	// attempt to open the file specified
	ofstream fout;

	// attempt to open the file using binary access
	fout.open(filename, ios::binary);

	unsigned int number_of_bytes(m_info.biWidth * m_info.biHeight * 4);
	BYTE red(0), green(0), blue(0);

	if(fout.is_open())
	{
		// same as before, only outputting now
		fout.write((char *)(&m_header), sizeof(BITMAPFILEHEADER));
		fout.write((char *)(&m_info), sizeof(BITMAPINFOHEADER));

		// read off the color data in the bass ackwards MS way
		for(unsigned int index(0); index < number_of_bytes; index += 4)
		{
			red = m_rgba_data[index];
			green = m_rgba_data[index + 1];
			blue = m_rgba_data[index + 2];

			fout.write((const char *)(&blue), sizeof(blue));
			fout.write((const char *)(&green), sizeof(green));
			fout.write((const char *)(&red), sizeof(red));
		}

		// close the file
		fout.close();
	}
	else
	{
		// post file not found message
		cout << "What is this " << filename << " you speak of?";
	}
}
