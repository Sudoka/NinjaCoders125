#pragma once
#include "PhysicsEngine.h"
#include "defs.h"
#include "Frame.h"
#include <vector>
//#define UNITOFHALFLENGTH 25		//as in half the length of a box

//All physics data should be known to the frames
struct PhysicsModel
{
	PhysicsModel(Point_t pos, Quat_t rot, float mass) {
		ref = new Frame(pos,rot);
		vel = Vec3f();
		accel = Vec3f();
		this->mass = mass;
		frictCoeff = PE::get()->getGroundFrict();
		surfaceId = 0;
	}

	virtual ~PhysicsModel() {
		delete ref;
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

	Frame *ref;	//Frame of Reference/skeleton; also root position and collision info
	Vec3f vel;			//Current velocity
	Vec3f accel;		//Current acceleration
	Vec3f lastPos;		//last position
	float mass;			//Mass of this object
	float frictCoeff;	//Friction coefficient
	uint surfaceId;		//ID of the object that this one is standing on
	//Vec3f frictNorm;	//Normal on which the friction will be applied
};
