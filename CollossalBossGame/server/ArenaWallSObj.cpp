#include "ArenaWallSObj.h"
#include "ServerObjectManager.h"
#include "ConfigurationManager.h"
#include "defs.h"


ArenaWallSObj::ArenaWallSObj(uint id, const char* filename, Model modelNum, Point_t pos, DIRECTION dir) : ServerObject(id) {
	Quat_t rot = Quat_t();
	pm = new PhysicsModel(pos, rot, 500);
	

	float fxo, fyo, fzo;
	const float roomWidth = CM::get()->find_config_as_float("ROOM_WIDTH"),
				roomHeight = CM::get()->find_config_as_float("ROOM_HEIGHT"),
				roomLength = CM::get()->find_config_as_float("ROOM_DEPTH");
	float scale;
	int div;
	switch(dir) {
	case NORTH:
		fxo = -roomWidth / 2;
		fyo = 0;
		fzo = -roomHeight / 2;
		scale = 60.0f / 255.0f;
		div = 5;
		break;
	default:
		fxo = -roomWidth / 2;
		fyo = 0;
		fzo = -roomLength / 2;
		scale = 12.5 / 255.0f;
		div = 5;
	}

	HMapElement *el = new HMapElement(filename, Point_t(fxo, fyo, fzo), div, scale, dir);	//Cleaned by the collision model
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
	ObjectState *state = (ObjectState*)buf;
	state->modelNum = modelNum;

	if (SOM::get()->collisionMode) {
		CollisionState *collState = (CollisionState*)(buf + sizeof(ObjectState));

		collState->totalBoxes = 0;

		return pm->ref->serialize(buf + sizeof(ObjectState) + sizeof(CollisionState)) + sizeof(ObjectState) + sizeof(CollisionState);
	} else {
		return pm->ref->serialize(buf + sizeof(ObjectState)) + sizeof(ObjectState);
	}

}
