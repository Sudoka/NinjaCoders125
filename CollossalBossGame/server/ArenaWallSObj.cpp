#include "ArenaWallSObj.h"
#include "ServerObjectManager.h"
#include "ConfigurationManager.h"
#include "defs.h"


ArenaWallSObj::ArenaWallSObj(uint id, const char* filename, Model modelNum, Point_t pos) : ServerObject(id) {
	Quat_t rot = Quat_t();
	pm = new PhysicsModel(pos, rot, 500, 0);
	
	const float fxo = -CM::get()->find_config_as_float("ROOM_WIDTH") / 2,
				fyo = 0,//CM::get()->find_config_as_float("ROOM_HEIGHT") / 2,
				fzo = -CM::get()->find_config_as_float("ROOM_DEPTH") / 2;

	HMapElement *el = new HMapElement(filename, Point_t(fxo, fyo, fzo), 6, 5.0f / 255.0f, UP);	//Cleaned by the collision model
	getCollisionModel()->add(el);

	this->modelNum = modelNum;
	this->setFlag(IS_STATIC, true);
	this->setFlag(IS_WALL, true);
}


ArenaWallSObj::~ArenaWallSObj(void) {
	delete pm;
}

bool ArenaWallSObj::update() {
	return false;
}

int ArenaWallSObj::serialize(char * buf) {
	return pm->ref->serialize(buf + sizeof(ObjectState)) + sizeof(ObjectState);
}
