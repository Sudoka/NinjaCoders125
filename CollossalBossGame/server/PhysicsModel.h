#pragma once
#include "defs.h"
#include "Frame.h"
#define AIR_FRICTION 1.1f	//A bit excessive, but it works for now
#define UNITOFHALFLENGTH 25		//as in half the length of a box

/* Bounding Enum
 *  Definition of all general types of collision. We map each model type to a boundary type
 *
 * Author: Bryan
 */
typedef enum CollisionBox {
	CB_SMALL,
	CB_LARGE,
	NUM_CBS
};

//All physics data should be known to the frames
struct PhysicsModel
{
	PhysicsModel(Point_t pos, Rot_t rot, float mass) {
		ref = new Frame(pos,rot);
//		appliedAccel = Vec3f();
		vel = Vec3f();
		accel = Vec3f();
		this->mass = mass;
		frictCoeff = AIR_FRICTION;
		this->colBox = CB_SMALL;		
	}

	virtual ~PhysicsModel() {
		delete ref;
	}

	void setColBox(CollisionBox cb)
	{
		this->colBox = cb;
	}

	void applyForce(const Vec3f &force) {
		this->accel.x += force.x / mass;
		this->accel.y += force.y / mass;
		this->accel.z += force.z / mass;
	}

	void applyAccel(const Vec3f &accel) {
		this->accel.x += accel.x;
		this->accel.y += accel.y;
		this->accel.z += accel.z;
	}
	
	/* getColBox
	 *
	 * Author: Bryan
	 */
	CollisionBox getColBox () {
		return colBox;
	}

	Frame *ref;	//Frame of Reference/skeleton; also root position and collision info
	Vec3f vel;			//Current velocity
	Vec3f accel;		//Current acceleration
	float mass;			//Mass of this object
	float frictCoeff;	//Friction coefficient
	Vec3f frictNorm;	//Normal on which the friction will be applied
	CollisionBox colBox;
};
