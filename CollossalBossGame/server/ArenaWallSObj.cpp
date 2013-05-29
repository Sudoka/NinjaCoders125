#include "ArenaWallSObj.h"
#include "ServerObjectManager.h"
#include "ConfigurationManager.h"
#include "defs.h"


ArenaWallSObj::ArenaWallSObj(uint id, const char* filename, Model modelNum, Point_t pos, DIRECTION dir) : ServerObject(id) {
	Quat_t rot = Quat_t();
	pm = new PhysicsModel(pos, rot, 500);
	DC::get()->print("New ArenaWallSObj created with id %d and direction ", id);

	float fxo, fyo, fzo;
	const float roomWidth = CM::get()->find_config_as_float("ROOM_WIDTH"),
				roomHeight = CM::get()->find_config_as_float("ROOM_HEIGHT"),
				roomLength = CM::get()->find_config_as_float("ROOM_DEPTH");
	float scale;
	int div;
	switch(dir) {
	case NORTH:
		DC::get()->print(DEFAULT_FLAGS & ~TIMESTAMP, "NORTH\n");
		fxo = -roomWidth / 2;
		fyo = -85.f;
		fzo = -roomHeight / 2;
		scale = 110.0f / 255.0f;
		div = 5;
		break;
	default:
		DC::get()->print(DEFAULT_FLAGS & ~TIMESTAMP, "UP\n");
		fxo = -roomWidth / 2;
		fyo = 0;
		fzo = -roomLength / 2;
		scale = 12.5 / 255.0f;
		div = 5;
	}

	HMapElement *el = new HMapElement(filename, Point_t(fxo, fyo, fzo), div, scale, dir);	//Cleaned by the collision model
	getCollisionModel()->add(el);

	//Add other collision elements
	switch(dir) {
	case NORTH:
		addNorthBoxes();
		break;
	default:
		addUpBoxes();
	}

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


void ArenaWallSObj::addNorthBoxes() {
	CollisionModel *cm = getCollisionModel();
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_LEFT_3")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_3")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_TOP_3")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_LEFT_2")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_2")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_TOP_2")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_LEFT_1")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_1")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_TOP_1")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PILLAR_1")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PILLAR_2")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PILLAR_3")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PILLAR_4")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PILLAR_5")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PLATFORM_LO")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PLATFORM_HI")));
}

void ArenaWallSObj::addSouthBoxes() {
	CollisionModel *cm = getCollisionModel();
	Quat_t rotAxis = Quat_t(Vec3f(0,1,0), (float)-M_PI);

	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_LEFT_3").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_3").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_TOP_3").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_LEFT_2").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_2").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_TOP_2").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_LEFT_1").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_1").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_TOP_1").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PILLAR_1").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PILLAR_2").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PILLAR_3").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PILLAR_4").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PILLAR_5").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PLATFORM_LO").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PLATFORM_HI").rotate(rotAxis))));
}

void ArenaWallSObj::addEastBoxes() {
}

void ArenaWallSObj::addWestBoxes() {
}


void ArenaWallSObj::addUpBoxes() {
}

void ArenaWallSObj::addDownBoxes() {
}