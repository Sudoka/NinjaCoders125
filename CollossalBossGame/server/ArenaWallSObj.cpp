#include "ArenaWallSObj.h"
#include "ServerObjectManager.h"
#include "ConfigurationManager.h"
#include "defs.h"


ArenaWallSObj::ArenaWallSObj(uint id, const char* filename, Model modelNum, Point_t pos, DIRECTION dir) : ServerObject(id) {
	Quat_t rot = Quat_t();
	pm = new PhysicsModel(pos, rot, 500);
	DC::get()->print("New ArenaWallSObj created with id %d and collision direction ", id);

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
	case SOUTH:
		DC::get()->print(DEFAULT_FLAGS & ~TIMESTAMP, "SOUTH\n");
		fxo = -roomWidth / 2;
		fyo = -70.f;
		fzo = -roomHeight / 2;
		scale = 110.0f / 255.0f;
		div = 5;
		break;
	case EAST:
		DC::get()->print(DEFAULT_FLAGS & ~TIMESTAMP, "EAST\n");
		fxo = -roomWidth / 2;
		fyo = -85.f;
		fzo = -roomHeight / 2;
		scale = 110.0f / 255.0f;
		div = 5;
		break;
	case WEST:
		DC::get()->print(DEFAULT_FLAGS & ~TIMESTAMP, "WEST\n");
		fxo = -roomWidth / 2;
		fyo = -85.f;
		fzo = -roomHeight / 2;
		scale = 110.0f / 255.0f;
		div = 5;
		break;
	case DOWN:	//ceiling
		DC::get()->print(DEFAULT_FLAGS & ~TIMESTAMP, "DOWN (ceiling)\n");
		fxo = -roomWidth / 2;
		fyo = -85.f;
		fzo = -roomHeight / 2;
		scale = 110.0f / 255.0f;
		div = 5;
		break;
	default:	//floor
		DC::get()->print(DEFAULT_FLAGS & ~TIMESTAMP, "UP (floor)\n");
		fxo = -roomWidth / 2;
		fyo = 0;
		fzo = -roomLength / 2;
		scale = 12.5f / 255.0f;
		div = 5;
	}

	HMapElement *el = new HMapElement(filename, Point_t(fxo, fyo, fzo), div, scale, dir);	//Cleaned by the collision model
	getCollisionModel()->add(el);

	//Add other collision elements
	switch(dir) {
	case NORTH:
		addNorthBoxes();
		break;
	case SOUTH:
		addSouthBoxes();
		break;
	case EAST:
		addEastBoxes();
		break;
	case WEST:
		addWestBoxes();
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

		vector<CollisionElement*>::iterator cur = getCollisionModel()->getStart(),
			end = getCollisionModel()->getEnd();

		int i = 0;
		while(cur < end && i < maxBoxes) {
			if ((*cur)->getType() == CMDL_AABB)
			{
				//The collision box is relative to the object's frame-of-ref.  Get the non-relative collision box
				collState->boxes[i] = ((AabbElement*)(*cur))->bx + pm->ref->getPos();
				i++; // update counter
			}

			cur++; // update our iterator
		}

		collState->totalBoxes = i;

		return pm->ref->serialize(buf + sizeof(ObjectState) + sizeof(CollisionState)) + sizeof(ObjectState) + sizeof(CollisionState);
	} else {
		return pm->ref->serialize(buf + sizeof(ObjectState)) + sizeof(ObjectState);
	}

}


void ArenaWallSObj::addNorthBoxes() {
	CollisionModel *cm = getCollisionModel();
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PLATFORM_LO")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PLATFORM_HI")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PILLAR_1")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PILLAR_2")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PILLAR_3")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PILLAR_4")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PILLAR_5")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_LEFT_3")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_3")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_TOP_3")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_LEFT_2")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_2")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_TOP_2")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_LEFT_1")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_1")));
	cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_FRAME_TOP_1")));
}

void ArenaWallSObj::addSouthBoxes() {
	CollisionModel *cm = getCollisionModel();
	Quat_t rotAxis = Quat_t(Vec3f(0,1,0), (float)-M_PI);

	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PLATFORM_LO").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PLATFORM_HI").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PILLAR_1").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PILLAR_2").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PILLAR_3").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PILLAR_4").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_PILLAR_5").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_LEFT_3").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_3").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_TOP_3").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_LEFT_2").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_2").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_TOP_2").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_LEFT_1").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_RIGHT_1").rotate(rotAxis))));
	cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_FRAME_TOP_1").rotate(rotAxis))));
}

void ArenaWallSObj::addEastBoxes() {
}

void ArenaWallSObj::addWestBoxes() {
}


void ArenaWallSObj::addUpBoxes() {
}

void ArenaWallSObj::addDownBoxes() {
}