#include "CollisionModel.h"
#include "defs.h"
#include <math.h>

/*
 * CollisionModel functions
 */
CollisionModel::~CollisionModel() {
	clean();
}

int CollisionModel::add(CollisionElement *ce) {
	vCollisionElements.push_back(ce);
	return vCollisionElements.size() - 1;
}

CollisionElement *CollisionModel::get(int i) {
	return vCollisionElements[i];
}

void CollisionModel::clean() {
	for(vector<CollisionElement*>::iterator iter = vCollisionElements.begin();
			iter < vCollisionElements.end();
			++iter) {
		delete *iter;
	}
	vCollisionElements.clear();
}

/*
 * AabbElement functions
 */
AabbElement::AabbElement(float x, float y, float z, float w, float h, float l) {
	bx = Box(x, y, z, w, h, l);
}

AabbElement::AabbElement(const Box &bx) {
	this->bx = bx;
}

/*
 * HMapElement functions
 */
HMapElement::HMapElement(const char *filename, const Vec3f &offset, int unitLength, float scale, DIRECTION normalDir) {
	bCreatedHMap = true;
	init(new HMap(filename, unitLength, scale), offset, normalDir);
}

//We're ignoring the normal direction atm
HMapElement::HMapElement(HMap *hmap, const Vec3f &offset, DIRECTION normalDir) {
	bCreatedHMap = false;
	init(hmap, offset, normalDir);				
}

void HMapElement::init(HMap *hmap, const Vec3f &offset, DIRECTION normalDir) {
	this->hmap = hmap;
	this->dir = normalDir;
	float max = hmap->getMax();
	this->bxTotalVolume = Box(offset.x, offset.y, offset.z,
		(float)hmap->getWidth() * hmap->getUnitLength(), max, (float)hmap->getLength() * hmap->getUnitLength());
}

HMapElement::~HMapElement() {
	if(bCreatedHMap) {
		delete hmap;
	}
}

/*
 * Collision detection functions
 */
bool areColliding(const Box &bx1, const Box &bx2) {
	return !(bx1.x + bx1.w < bx2.x ||
			 bx1.y + bx1.h < bx2.y ||
			 bx1.z + bx1.l < bx2.z ||
			 bx1.x > bx2.x + bx2.w ||
			 bx1.y > bx2.y + bx2.h ||
			 bx1.z > bx2.z + bx2.l);
}

bool pointOnHMapCollision(float *hdiff, const Point_t &pt, const Point_t &hmapPos, const HMapElement &hmap) {
	//Transform point so it is relative to the hmap top-left corner
	float x = (pt.x - (hmapPos.x + hmap.bxTotalVolume.x)) / hmap.hmap->getUnitLength(),
		  y = (pt.y - (hmapPos.y + hmap.bxTotalVolume.y)),	//Don't normalize the height
		  z = (pt.z - (hmapPos.z + hmap.bxTotalVolume.z)) / hmap.hmap->getUnitLength();
	
	//Get indices of corners
	int minI = (int)floor(x),
		minJ = (int)floor(z),
		maxI = (int)ceil(x),
		maxJ = (int)ceil(z);

	//Determine which triangle to check
	Vec3f v0, v1, norm;
	float h00 = hmap.hmap->getHeightAt(minI,minJ),
		  h11 = hmap.hmap->getHeightAt(maxI,maxJ);
	if(z - minJ < x - minI) {
		//Get two vectors in the plane of the triangle, such that crossing them gets the correct normal
		float h10 = hmap.hmap->getHeightAt(maxI,minJ);
		v0 = Vec3f(-1, h00 - h10, 0);
		v1 = Vec3f(0, h11 - h10, 1);
	} else {
		float h01 = hmap.hmap->getHeightAt(maxI,minJ);
		v0 = Vec3f(0, h11 - h01, 1);
		v1 = Vec3f(-1, h00 - h01, 0);
	}
	Point_t ptOnPlane = Point_t((float)minI,h00,(float)minJ);	//guaranteed to be on both planes
	cross(&norm, v0, v1);
	norm.normalize();	//We only normalize this because it might serve as the collision normal later

	//now that we have the normal and a point pt on the plane, we can use the
	// equation of the plane to determine whether the point is above or below
	// the heightmap.  If it's above, then there is no collision.  If its
	// below, then a collision occurred.
	// Since we have all of the information here, we might even return the collision normal
	// and height difference as well.

	//Equation of a plane: norm * (pt - ptOnPlane), except using adjusted pt values.
	*hdiff = ptOnPlane.y - (norm.x * (x - ptOnPlane.x) + norm.z * (z - ptOnPlane.z)) / norm.y	//height of the hmap at the current position
		- y;	//The current player's y-position, translated so it is relative the hmap's topleft-most corner
	return *hdiff >= 0;	//hdiff is the amount of shift that needs to happen to move the object out of the heightmap.
}

bool areColliding(Vec3f *shift, DIRECTION *collDir, const Box &bx, const Point_t &hmapCenter, const HMapElement &hmap) {
	Point_t objPos = Point_t(), hmapPos = Point_t();
	float hdiff;
	*collDir = hmap.dir;

	//Rotate x, y, z values so that the hmap shift direction is on the y axis
	switch(hmap.dir) {
	case NORTH:
		break;
	case SOUTH:
		break;
	case EAST:
		break;
	case WEST:
		break;
	case DOWN:
		break;
	default:	//UP
		//No rotations
		objPos = Point_t(bx.x + bx.w / 2, bx.y, bx.z + bx.l / 2);	//Bottom of the box
		hmapPos = hmapCenter;
		break;
	}
	
	if(pointOnHMapCollision(&hdiff, objPos, hmapPos, hmap)) {
		switch(hmap.dir) {
		case NORTH:
			*shift = Point_t(0, hdiff, 0);
			break;
		case SOUTH:
			*shift = Point_t(0, hdiff, 0);
			break;
		case EAST:
			*shift = Point_t(0, hdiff, 0);
			break;
		case WEST:
			*shift = Point_t(0, hdiff, 0);
			break;
		case DOWN:
			*shift = Point_t(0, hdiff, 0);
			break;
		default:	//UP
			//No rotations
			*shift = Point_t(0, hdiff, 0);
			break;
		}
		return true;
	}
	return false;
}

/*
 * Collision handling functions
 */

/*
 * Extract the shift axis and magnitude, as well as the collision normal
 * The calling function is responsible for deciding which box moves which amount.
 * Assumes that a collision has already been detected.
 */
void getCollisionInfo(Vec3f *shift, DIRECTION *collDir, const Box &bx1, const Box &bx2) {
	//Move out bounding boxes if collision occurs
	float fXShift1 = bx2.x - (bx1.x + bx1.w),
          fXShift2 = (bx2.x + bx2.w) - bx1.x,
          fXShift  = fabs(fXShift1) < fabs(fXShift2) ? fXShift1 : fXShift2;
    float fYShift1 = bx2.y - (bx1.y + bx1.h),
          fYShift2 = (bx2.y + bx2.h) - bx1.y,
          fYShift  = fabs(fYShift1) < fabs(fYShift2) ? fYShift1 : fYShift2;
    float fZShift1 = bx2.z - (bx1.z + bx1.l),
          fZShift2 = (bx2.z + bx2.l) - bx1.z,
          fZShift  = fabs(fZShift1) < fabs(fZShift2) ? fZShift1 : fZShift2;

	
	*shift = Vec3f();	//Clear the shift and normal vectors
	float sign = 0.0f;
    if(fabs(fXShift) < fabs(fYShift) && fabs(fXShift) < fabs(fZShift)) {
        //Shift by X
		shift->x = fXShift;
		*collDir = ((fXShift < 0) || ((fXShift == 0) && (bx1.x < bx2.x))) ? WEST : EAST;
    } else if(fabs(fYShift) < fabs(fXShift) && fabs(fYShift) < fabs(fZShift)) {
        //Shift by Y (vertical)
		shift->y = fYShift;
		*collDir = ((fYShift < 0) || ((fYShift == 0) && (bx1.y < bx2.h))) ? DOWN : UP;
    } else {
        //Shift by Z
		shift->z = fZShift;
		*collDir = ((fZShift < 0) || ((fZShift == 0) && (bx1.z < bx2.z))) ? SOUTH : NORTH;
	}
}

void getClosestPoints(Point_t *pt1, Point_t *pt2, float *time,
		const Point_t &ipt1, const Point_t &fpt1,
		const Point_t &ipt2, const Point_t &fpt2) {
	//Calculate all of the vectors and dot products we will need
	Vec3f dir1 = fpt1 - ipt1,
		  dir2 = fpt2 - ipt2,
		  diff = ipt1 - ipt2;

	/*
	 * Equation of a line: dir*t + ipt, where t is a scalar parameter.
	 * The minimum distance between two lines in 3D space is found by
	 * finding the minimum of the distance between two points on the
	 * line for any two points.  Since t directly corresponds to time,
	 * we can use the same parameter for both distance equations, and
	 * thus greatly simplify the problem.
	 */
	float dir1DOTdir1 = dir1 ^ dir1,	//magnitude of dir1, squared
		  dir1DOTdir2 = dir1 ^ dir2,
		  dir2DOTdir2 = dir2 ^ dir2,	//magnitude of dir2, squared
		  dir2DOTdiff = dir2 ^ diff,
		  dir1DOTdiff = dir1 ^ diff;
	float t, num, den;
	den = dir1DOTdir1 - 2 * dir1DOTdir2 + dir2DOTdir2;
	if(den != 0) {
		num = dir2DOTdiff - dir1DOTdiff;
		t = num / den;

		//Limit calculated t value
		if(t < 0.5f || t > 1.0f) t = 1.0f;
	} else {
		t = 1.0f;
	}

	*pt1 = ipt1 + dir1 * t;
	*pt2 = ipt2 + dir2 * t;
	*time = t;
}
