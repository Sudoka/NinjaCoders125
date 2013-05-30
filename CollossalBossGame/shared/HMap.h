#pragma once

/*
 * HMap
 * This class is used for reading in and storing information about heightmaps.
 */
using namespace std;

class HMap {
public:
	HMap(const char * filename, int unitLength, float scale = 1.0f);
	virtual ~HMap(void);

	// Number of units wide
	int getWidth() { return _w; }

	// Number of units long
	int getLength() { return _l; }

	int getUnitLength() { return _ul; }

	// returns a long int with rgba data for point (x, y)
	float getHeightAt(int i, int j);
	const float *getIterator() { return _hdata; }

	float getMax() { return _max; }
	float getMin() { return _min; }

private:
	int _w, _l, _ul;

	float _max, _min;

	// pixel data
	float * _hdata;
};
