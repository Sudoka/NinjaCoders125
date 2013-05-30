#include "Frame.h"

Frame::Frame()
{
	this->pos = Point_t();
	this->rot = Quat_t();
}

Frame::Frame(const Point_t &pos, const Quat_t &rot)
{
	this->pos = pos;
	this->rot = rot;
}


Frame::~Frame(void)
{
}

#if 0
void Frame::addChild(Frame* newChild)
{
	lsAttachedFrames.push_back(newChild);
}
#endif


void Frame::translate(const Vec3f &dv) {
	pos.x += dv.x;
	pos.y += dv.y;
	pos.z += dv.z;
}

void Frame::rotate(const Quat_t &dr) {
	rot *= dr;
	rot.normalize();
}

int Frame::serialize(char * buf) {
	StateMessage *st = (StateMessage*)buf;
	st->pos = pos;
	st->rot = rot;
	return sizeof(st);
}

void Frame::deserialize(char* newState) {
	StateMessage* state = (StateMessage*)newState;
	this->pos = state->pos;
	this->rot = state->rot;
}
