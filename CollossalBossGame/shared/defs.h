/*
 * defs.h
 * Standard definitions and includes that would be useful for most objects
 */
#ifndef DEFS_H
#define DEFS_H

//Standard includes
#include <stdio.h>
#include "DebugConsole.h"
#include "Windows.h"
#include <set>
#include <math.h>

//Constants (that we don't want to change, if we might, they should go in the config file)
#define M_PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286
#define M_TAU (2 * M_PI)

//Macros
#define GET_FLAG(flags, flag)        ((flags >> flag) & 0x1)
#define SET_FLAG(flags, flag, val)   ((val) ? (flags | (1 << flag)) : (flags & ~(1 << flag)))

// Forward Declarations (definitions found in this file)
struct Vec3f;
struct Vec4f;
struct Box;

// Standalone functions
float magnitude(const Vec3f &v);
float magnitude(const Vec4f &v);

//Typedefs
typedef unsigned int uint;

//Types
/*
 * Vec3f: Points, Euler rotations
 */
typedef struct Vec3f {
	Vec3f();
	Vec3f(float x, float y, float z);
	
	float x, y, z;

	//Constant operators: Creates a new point structure
	bool operator< (const Vec3f &rhs) const;	//Point comparison
	Vec3f operator-	(const Vec3f &rhs) const;	//Vector division
	Vec3f operator+	(const Vec3f &rhs) const;	//Vector sum
	float operator^	(const Vec3f &rhs) const;	//Dot product
	Vec3f operator*	(const Vec3f &rhs) const;	//Dot product without summing
	Vec3f operator/	(const Vec3f &rhs) const;	//Dot division without summing
	Vec3f operator*	(float rhs) const;
	Vec3f operator/	(float rhs) const;

	//Operations that modify the structure
	void operator+= (const Vec3f &rhs);
	void operator-= (const Vec3f &rhs);
	void operator*= (float s);
	void operator/= (float s);
	void normalize();
} Point_t, Rot_t;

/*
 * Vec4f: Quaternions
 */
typedef struct Vec4f {
	//   (0, 1, 2, 3)
	float w, x, y, z;

	Vec4f();									//Default zero quaternion (x = y = z = 0, w = 1)
	Vec4f(float x, float y, float z, float w);	//Fill the vector however you like
	Vec4f(const Vec3f &axis, float angle);		//Quaternion with a rotation about an axis

	//Operations
	void operator*=(const Vec4f &rhs);			//Quaternion concatenation
	Vec4f operator*(const Vec4f &rhs) const;	//Quaternion concatenation, creates a new quaternion
	Vec3f extractAxis();

	inline void normalize() {
		float mag = sqrt(x * x + y * y + z * z + w * w);
		x /= mag;
		y /= mag;
		z /= mag;
		w /= mag;
	}

	/* Given a vector of the change we want to make,
	 *  and the actual axis we are currently on,
	 *  make the appropriate change in this vector.
	 *
	 * Author: Bryan
	 */
	Vec3f rotateToThisAxis(Vec3f change);
} Quat_t;


/*
 * For communicating it's state across the server
 */
struct CollisionBoxState {
	Vec3f position;
	Vec3f dimensions;
};

//Axis-aligned bounding box
typedef struct Box {
	float x, y, z;
	float w, h, l;

	Box();
	Box(float x, float y, float z, float w, float h, float l);

	//Operators
	Box operator+ (const Vec3f &pt) const;
	Box operator- (const Box &bx) const;
	Box* fix();
	Box* rotate(Vec4f axis);
	void setPos(const Vec3f &pos);
	void setRelPos(const Vec3f &pos);
	void setSize(const Vec3f &size);
	void setRelSize(const Vec3f &size);

	inline Vec3f getPos() const {
		return Vec3f(x, y, z);
	}

	inline Vec3f getSize() const {
		return Vec3f(w, h, l);
	}

	inline Vec3f getCenter() const {
		return Vec3f(x + w / 2, y + h / 2, z + l / 2);
	}
} Vol_t;

//Enumerations
/*
 * Actions that are sent from the client to the server
 */

typedef enum OBJ_FLAG {
	//General flags
	IS_HEALTHY,
	IS_HARMFUL,
	IS_WALL,

	//Physics flags
	IS_STATIC,
	IS_PASSABLE,
	IS_FALLING,
	IS_FLOATING,
	IS_IGNORE_INPUT
};

typedef enum DIRECTION {
	NONE  = 0x0,
	NORTH = 0x1,	//+z
	EAST  = 0x2,	//+x
	UP    = 0x4,	//+y
	SOUTH = 0x8,	//-z
	WEST  = 0x10,	//-x
	DOWN  = 0x20	//-y
};

inline DIRECTION flip(DIRECTION dir) {
	return (DIRECTION)((dir < 0x7) ? (dir << 3) : (dir >> 3));
}
Vec3f dirVec(DIRECTION dir);	//Gets a vector in the direction specified
Vec3f dirAxis(DIRECTION dir);		//Gets the (positive) axis for the specified direction


typedef enum ACTION {
	ACT_MOVE_X,
	ACT_MOVE_Y,
	ACT_TURN_X,
	ACT_TURN_Y,
	ACT_FOCUS,
	ACT_JUMP,
	ACT_SPECIAL,
	ACT_ATTACK,
	ACT_NUM_ACTIONS
};

struct GameData {
	int start;
	int hardreset;
	int left;
	int right;
	
	int clientready;

	int playerid;

	void clear() {
		SecureZeroMemory(this, sizeof(GameData));
	}
};

//Standalone functions
Quat_t inverse(const Quat_t &q);
float angle(const Vec3f &v1, const Vec3f &v2);
Vec3f rotate(const Vec3f &v, const Quat_t &q);
void  getCorrectedAxes(const Quat_t &q, Vec3f *fwd, Vec3f *up, Vec3f *right);
Vec3f rotateUp(const Quat_t &q);
Vec3f rotateRight(const Quat_t &q);
Vec3f rotateFwd(const Quat_t &q);
void cross(Vec3f *res, const Vec3f &v1, const Vec3f &v2);
void slerp(Quat_t *res, const Quat_t &start, const Quat_t &end, float t);

#endif