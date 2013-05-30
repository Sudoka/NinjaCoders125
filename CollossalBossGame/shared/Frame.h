/*
 * Frame.h
 * This defines a frame of reference
 *
 * Author(s): Nathan, Haro, Suman
 */
#pragma once
#include "defs.h"
#include <list>
using namespace std;

struct StateMessage {
	Point_t pos;
	Quat_t rot;
};

class Frame {
public:
	Frame();
	Frame(const Point_t &pos, const Quat_t &rot);
	virtual ~Frame(void);
	//void addChild(Frame* newChild);
	
	void translate(const Vec3f &dv);
	void rotate(const Quat_t &dr);

	void setPos(const Point_t &pt) {
		pos = pt;
	}
	void setRot(const Quat_t &rt) { rot = rt; }
	Point_t getPos() { return pos; }
	Quat_t getRot() { return rot; }

	int serialize(char * buf);
	void deserialize(char* newState);
	//stateMessage st;
private:
	Point_t pos;
	Quat_t rot;
	//list<Frame*> lsAttachedFrames;
};

