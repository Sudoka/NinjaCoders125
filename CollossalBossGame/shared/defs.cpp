/*
 * defs.cpp
 * Definitions for methods defined in defs.h
 */
#include "defs.h"


/*
 * Vec3f functions
 */
Vec3f::Vec3f() {
	x = y = z = 0.f;
}

Vec3f::Vec3f(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

bool Vec3f::operator< (const Vec3f &rhs) const {
	// First, check magnitude
	float res =	magnitude(*this) - magnitude(rhs);
	if (res != 0) return res < 0;

	// If same, check each component (rather arbitrarily)
	if (this->z != rhs.z) return this->z < rhs.z;
	if (this->x != rhs.x) return this->x < rhs.x;
	return this->y < rhs.y;
}

Vec3f Vec3f::operator-	(const Vec3f &rhs) const {
	return Vec3f(this->x - rhs.x, 
		this->y - rhs.y, 
		this->z - rhs.z);
}
	
Vec3f Vec3f::operator+	(const Vec3f &rhs) const {
	return Vec3f(this->x + rhs.x, 
		this->y + rhs.y, 
		this->z + rhs.z);
}

void Vec3f::operator+= (const Vec3f &rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
}

void Vec3f::operator-= (const Vec3f &rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
}

Vec3f Vec3f::operator*	(float rhs) const {
	return Vec3f(this->x * rhs, 
		this->y * rhs, 
		this->z * rhs);
}

Vec3f Vec3f::operator/	(float rhs) const  {
	if (rhs == 0)
		return Vec3f(0, 0, 0);
	else
		return Vec3f(this->x / rhs, 
			this->y / rhs, 
			this->z / rhs);
}

//Basically dot product
Vec3f Vec3f::operator*	(const Vec3f &rhs) const {
	return Vec3f(this->x * rhs.x, 
		this->y * rhs.y, 
		this->z * rhs.z);
}

// DOT PRODUCT!
float Vec3f::operator^	(const Vec3f &rhs) const {
	return (this->x * rhs.x +
		this->y * rhs.y + 
		this->z * rhs.z);
}

//Basically dot-division
Vec3f Vec3f::operator/	(const Vec3f &rhs) const {
	float x = (rhs.x == 0) ? 0 : this->x / rhs.x;
	float y = (rhs.y == 0) ? 0 : this->y / rhs.y;
	float z = (rhs.z == 0) ? 0 : this->z / rhs.z;
	return Vec3f(x, y, z);
}

//Scalar ops
void Vec3f::operator*= (float s) {
	x *= s;
	y *= s;
	z *= s;
}

void Vec3f::operator/= (float s) {
	x /= s;
	y /= s;
	z /= s;
}
void Vec3f::normalize() {
	float mag = sqrt(x * x + y * y + z * z);
	x /= mag;
	y /= mag;
	z /= mag;
}


/*
 * Vec4f functions
 */


Vec4f::Vec4f() {
	x = y = z = 0.0f;
	w = 1.0f;
}

Vec4f::Vec4f(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vec4f::Vec4f(const Vec3f &axis, float angle) {
	float s = cos(angle / 2),
			t = sin(angle / 2);
	this->w = s;
	this->x = axis.x * t;
	this->y = axis.y * t;
	this->z = axis.z * t;
	normalize();
}

//http://www.mathworks.com/help/aeroblks/quaternionmultiplication.html
void Vec4f::operator*=(const Vec4f &rhs) {
	float w2 = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
		x2 = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
		y2 = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
		z2 = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
	w = w2;
	x = x2;
	y = y2;
	z = z2;
}

Vec4f Vec4f::operator*(const Vec4f &rhs) const {
	Vec4f res = *this;
	res *= rhs;
	return res;
}

Vec3f Vec4f::extractAxis() {
	float halfAngle = acos(this->w);
	return Vec3f(this->x / halfAngle, this->y / halfAngle, this->z / halfAngle);
}

/*
 * Box functions
 */
Box::Box() {
	x = y = z = w = l = h = 0;
}
	
Box::Box(float x, float y, float z, float w, float h, float l) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
	this->h = h;
	this->l = l;
}

Box Box::operator+ (const Vec3f &pt) const {
	return Box(x + pt.x, y + pt.y, z + pt.z,
				w,        h,        l);
}

Box Box::operator- (const Box &bx) const {
	return Box(x - bx.x, y - bx.y, z - bx.z,
				w - bx.w, h - bx.h, l - bx.l);
}

Box* Box::fix() {
	// This part works with negative height, width, length
	if (this->w < 0 || this->h < 0 || this->l < 0)
	{
		Vec3f minCorner = Vec3f(	min(this->x + this->w, this->x), 
									min(this->y + this->h, this->y), 
									min(this->z + this->l, this->z));

		Vec3f maxCorner = Vec3f(	max(this->x + this->w, this->x), 
									max(this->y + this->h, this->y), 
									max(this->z + this->l, this->z));

		this->x = minCorner.x;
		this->y = minCorner.y; 
		this->z = minCorner.z,
		this->w = maxCorner.x - minCorner.x;
		this->h = maxCorner.y - minCorner.y;
		this->l = maxCorner.z - minCorner.z;
	}

	return this;
}

Box* Box::rotate(Vec4f axis) {
	this->setPos(axis.rotateToThisAxis(this->getPos()));
	this->setSize(axis.rotateToThisAxis(this->getSize()));
	return this->fix();
}

void Box::setPos(const Vec3f &pos) {
	x = pos.x; y = pos.y; z = pos.z;
}

void Box::setRelPos(const Vec3f &pos) {
	x += pos.x; y += pos.y; z += pos.z;
}

void Box::setSize(const Vec3f &size) {
	w = size.x; h = size.y; l = size.z;
}

void Box::setRelSize(const Vec3f &size) {
	w += size.x; h += size.y; l += size.z;
}


/*
 * Standalone functions
 */
Quat_t inverse(const Quat_t &q) {
	return Quat_t(-q.x, -q.y, -q.z, q.w);
}

float magnitude(const Vec3f &v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float magnitude(const Vec4f &v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

Vec3f rotate(const Vec3f &v, const Quat_t &q) {
	//Convert the quaternion to matrix values
	float f00, f01, f02,
		  f10, f11, f12,
		  f20, f21, f22;
	f00 = 1 - 2 * q.y * q.y - 2 * q.z * q.z;	//1 - 2y^2 - 2z^2
	f01 = 2 * q.x * q.y - 2 * q.w * q.z;		//2xy - 2wz
	f02 = 2 * q.x * q.z + 2 * q.w * q.y;		//2xz + 2wy
	f10 = 2 * q.x * q.y + 2 * q.w * q.z;		//2xy + 2wz
	f11 = 1 - 2 * q.x * q.x - 2 * q.z * q.z;	//1 - 2x^2 - 2z^2
	f12 = 2 * q.y * q.z - 2 * q.w * q.x;		//2yz - 2wx
	f20 = 2 * q.x * q.z - 2 * q.w * q.y;		//2xz - 2wy
	f21 = 2 * q.y * q.z + 2 * q.w * q.x;		//2yz + 2wx
	f22 = 1 - 2 * q.x * q.x - 2 * q.y * q.y;	//1 - 2x^2 - 2y^2

	//Perform matrix multiplication
	return Vec3f(
			v.x * f00 + v.y * f01 + v.z * f02,
			v.x * f10 + v.y * f11 + v.z * f12,
			v.x * f20 + v.y * f21 + v.z * f22
		);
}

/*
 * Performs the same operation as rotate(Vec3f(0,1,0), q), but
 * simplifies the math so that fewer calculations need to be performed
 */
Vec3f rotateUp(const Quat_t &q) {
	//Convert the quaternion to matrix values
	float f01, f11, f21;
	f01 = 2 * q.x * q.y - 2 * q.w * q.z;		//2xy - 2wz
	f11 = 1 - 2 * q.x * q.x - 2 * q.z * q.z;	//1 - 2x^2 - 2z^2
	f21 = 2 * q.y * q.z + 2 * q.w * q.x;		//2yz + 2wx
	
	//Perform matrix multiplication
	Vec3f res = Vec3f(f01, f11, f21);	//only the y values are kept
	res.normalize();
	return res;
}

/*
 * Performs the same operation as rotate(Vec3f(1,0,0), q), but
 * simplifies the math so that fewer calculations need to be performed
 */
Vec3f rotateRight(const Quat_t &q) {
	//Convert the quaternion to matrix values
	float f00, f10, f20;
	f00 = 1 - 2 * q.y * q.y - 2 * q.z * q.z;	//1 - 2y^2 - 2z^2
	f10 = 2 * q.x * q.y + 2 * q.w * q.z;		//2xy + 2wz
	f20 = 2 * q.x * q.z - 2 * q.w * q.y;		//2xz - 2wy
	
	//Perform matrix multiplication
	Vec3f res = Vec3f(f00, f10, f20);	//only the x values are kept
	res.normalize();
	return res;
}

/*
 * Performs the same operation as rotate(Vec3f(0,0,1), q), but
 * simplifies the math so that fewer calculations need to be performed
 */
Vec3f rotateFwd(const Quat_t &q) {
	//Convert the quaternion to matrix values
	float f02, f12, f22;
	f02 = 2 * q.x * q.z + 2 * q.w * q.y;		//2xz + 2wy
	f12 = 2 * q.y * q.z - 2 * q.w * q.x;		//2yz - 2wx
	f22 = 1 - 2 * q.x * q.x - 2 * q.y * q.y;	//1 - 2x^2 - 2y^2

	//Perform matrix multiplication
	Vec3f res = Vec3f(f02, f12, f22);
	res.normalize();
	return res;
}


void getCorrectedAxes(const Quat_t &q, Vec3f *fwd, Vec3f *up, Vec3f *right) {
	*fwd = rotateFwd(q);
	*right = rotateRight(q);
	fwd->normalize();
	right->normalize();

	cross(up, *fwd, *right);
	up->normalize();
	
	cross(right, *up, *fwd);
	right->normalize();
}

void cross(Vec3f *res, const Vec3f &v1, const Vec3f &v2) {
	res->x = v1.y * v2.z - v1.z * v2.y;
	res->y = v1.z * v2.x - v1.x * v2.z;
	res->z = v1.x * v2.y - v1.y * v2.x;
}


float angle(const Vec3f &v1, const Vec3f &v2) {
	return acos((v1 ^ v2) / (magnitude(v1) * magnitude(v2)));
}

void slerp(Quat_t *res, const Quat_t &start, const Quat_t &end, float t) {
	res->w = end.w * (t) + start.w * (t - 1);
	res->x = end.x * (t) + start.x * (t - 1);
	res->y = end.y * (t) + start.y * (t - 1);
	res->z = end.z * (t) + start.z * (t - 1);
	res->normalize();
}


Vec3f Vec4f::rotateToThisAxis(Vec3f change) {
	return rotate(change, *this);
}

Vec3f dirVec(DIRECTION dir) {
	switch(dir) {
	case NORTH:
		return Vec3f(0,0,1);
	case EAST:
		return Vec3f(1,0,0);
	case UP:
		return Vec3f(0,1,0);
	case SOUTH:
		return Vec3f(0,0,-1);
	case WEST:
		return Vec3f(-1,0,0);
	//case DOWN:
	default:
		return Vec3f(0,-1,0);
	}
}

Vec3f dirAxis(DIRECTION dir) {
	switch(dir) {
	case NORTH:
	case SOUTH:
		return Vec3f(0,0,1);
	case EAST:
	case WEST:
		return Vec3f(1,0,0);
	//case UP:
	//case DOWN:
	default:
		return Vec3f(0,1,0);
	}
}
