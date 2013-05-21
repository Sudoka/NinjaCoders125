#include "WallSObj.h"
#include "ServerObjectManager.h"
#include "ConfigurationManager.h"
#include "defs.h"
#include <math.h>

#define WALL_WIDTH 2000 //150; increased size to reduce edge-case collision errors
#define WALL_THICKNESS 200

WallSObj::WallSObj(uint id, Model modelNum, Point_t pos, DIRECTION dir) : ServerObject(id) {
	if(SOM::get()->debugFlag) DC::get()->print("Created new WallSObj %d ", id);
	vector<Box> bxVols;
	Quat_t rot = Quat_t();
	uint collDir = dir;
	switch(dir) {
	case NORTH:
		DC::get()->print("(north)\n");
		bxVols.push_back(CM::get()->find_config_as_box("BOX_PILLAR_1"));
		bxVols.push_back(CM::get()->find_config_as_box("BOX_PILLAR_2"));
		bxVols.push_back(CM::get()->find_config_as_box("BOX_PILLAR_3"));
		bxVols.push_back(CM::get()->find_config_as_box("BOX_PILLAR_4"));
		bxVols.push_back(CM::get()->find_config_as_box("BOX_PILLAR_5"));
		bxVols.push_back(CM::get()->find_config_as_box("BOX_PLATFORM_LO"));
		bxVols.push_back(CM::get()->find_config_as_box("BOX_PLATFORM_HI"));
		bxVols.push_back(Box((-WALL_WIDTH / 2), -WALL_WIDTH / 2, -WALL_THICKNESS + 5,
			WALL_WIDTH, WALL_WIDTH, WALL_THICKNESS));
		collDir = NORTH;
		break;
	case SOUTH:
		DC::get()->print("(south)\n");
		bxVols.push_back(Box((-WALL_WIDTH / 2), -WALL_WIDTH / 2, 0 - 5 ,
			WALL_WIDTH, WALL_WIDTH, WALL_THICKNESS));
		collDir = SOUTH;
		break;
	case EAST:
		DC::get()->print("(east)\n");
		bxVols.push_back(Box(0 - 5, -WALL_WIDTH / 2, -WALL_WIDTH / 2,
			WALL_THICKNESS, WALL_WIDTH, WALL_WIDTH));
		collDir = WEST;
		break;
	case WEST:
		DC::get()->print("(west)\n");
		bxVols.push_back(Box(-WALL_THICKNESS, -WALL_WIDTH / 2, -WALL_WIDTH / 2,
			WALL_THICKNESS, WALL_WIDTH, WALL_WIDTH));
		collDir = EAST;
		break;
	case UP:
		DC::get()->print("(ceiling)\n");
		bxVols.push_back(Box(-WALL_WIDTH / 2, 0, -WALL_WIDTH / 2,
			WALL_WIDTH, WALL_THICKNESS, WALL_WIDTH));
		collDir = DOWN;
		break;
	default:
		DC::get()->print("(floor)\n");
		bxVols.push_back(Box(-WALL_WIDTH / 2, -WALL_THICKNESS + 5, -WALL_WIDTH / 2,
			WALL_WIDTH, WALL_THICKNESS, WALL_WIDTH));
		rot = Quat_t();
		collDir = UP;
		break;
	}

	pm = new PhysicsModel(pos, rot, 500, collDir);
	vector<Box>::iterator i;
	for(i = bxVols.begin(); i != bxVols.end(); i++) getCollisionModel()->add(new AabbElement(*i));

	this->modelNum = modelNum;
	//pm->setColBox(CB_FLAT);
	t = 0;
	this->setFlag(IS_STATIC, true);
	this->setFlag(IS_WALL, true);
}

WallSObj::~WallSObj(void) {
	delete pm;
}

bool WallSObj::update() {
	return false;
}

int WallSObj::serialize(char * buf) {
	ObjectState *state = (ObjectState*)buf;
	state->modelNum = modelNum;

	if (SOM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(buf + sizeof(ObjectState));

		vector<CollisionElement*>::iterator cur = getCollisionModel()->getStart(),
			end = getCollisionModel()->getEnd();

		collState->totalBoxes = min(end - cur, maxBoxes);

		for(int i=0; i < collState->totalBoxes; i++, cur++) {
			//The collision box is relative to the object's frame-of-ref.  Get the non-relative collision box
			collState->boxes[i] = ((AabbElement*)(*cur))->bx + pm->ref->getPos();
		}

		return pm->ref->serialize(buf + sizeof(ObjectState) + sizeof(CollisionState)) + sizeof(ObjectState) + sizeof(CollisionState);
	}
	else
	{
		return pm->ref->serialize(buf + sizeof(ObjectState)) + sizeof(ObjectState);
	}
}
