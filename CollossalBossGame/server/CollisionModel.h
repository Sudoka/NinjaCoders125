#pragma once

#include "defs.h"
#include "HMap.h"
#include <vector>
using namespace std;

enum CollisionType {
	CMDL_AABB,
	CMDL_HMAP,
	NUM_COLLISION_TYPES
};

/*
 * Note that all collision models are relative to a frame of reference, which is stored elsewhere
 */

class CollisionElement {
public:
	virtual CollisionType getType() = 0;
};

class CollisionModel {
public:
	~CollisionModel();

	int add(CollisionElement *ce);
	CollisionElement *get(int i);

	inline vector<CollisionElement*>::iterator getStart() { return vCollisionElements.begin(); }
	inline vector<CollisionElement*>::iterator getEnd()   { return vCollisionElements.end(); }
	
	void clean();

private:
	vector<CollisionElement*> vCollisionElements;
};


class AabbElement : public CollisionElement {
public:
	AabbElement(float x, float y, float z, float w, float h, float l);
	AabbElement(const Box &bx);

	virtual CollisionType getType() { return CMDL_AABB; }
	Box bx;

	inline void operator=(const AabbElement &el) {
		this->bx = el.bx;
	}

	inline void operator=(const Box &bx) {
		this->bx = bx;
	}
};

class HMapElement : public CollisionElement {
public:
	HMapElement(const char *filename, const Vec3f &offset, int unitLength, float scale, DIRECTION normalDir);
	HMapElement(HMap *hmap, const Vec3f &offset, DIRECTION normalDir);
	virtual ~HMapElement();

	virtual CollisionType getType() { return CMDL_HMAP; }
	HMap *hmap;
	Box bxTotalVolume;
	DIRECTION dir;

private:
	void init(HMap *hmap, const Vec3f &offset, DIRECTION normalDir);
	bool bCreatedHMap;
};

void getClosestPoints(Point_t *pt1, Point_t *pt2, float *t, const Point_t &ipt1, const Point_t &fpt1, const Point_t &ipt2, const Point_t &fpt2);

bool areColliding(const Box &bx1, const Box &bx2);
//bool areColliding(float *hdiff, const Box &bx, const Point_t &hmapCenter, const HMapElement &hmap);
bool areColliding(Vec3f *shift, DIRECTION *collDir, const Box &bx, const Point_t &hmapCenter, const HMapElement &hmap);

void getCollisionInfo(Vec3f *shift, DIRECTION *collDir, const Box &bx1, const Box &bx2);
