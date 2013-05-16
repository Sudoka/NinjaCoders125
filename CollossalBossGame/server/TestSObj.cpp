#include "TestSObj.h"
#include "ServerObjectManager.h"
#include <math.h>
#include "ConfigurationManager.h"

TestSObj::TestSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, int dir) : ServerObject(id) {
	if(SOM::get()->debugFlag) DC::get()->print("Created new TestSObj %d\n", id);
	setFlag(IS_FALLING,1);

	this->dir = dir;
	this->modelNum = modelNum;
	switch (modelNum) {
		case MDL_TEST_BOX:
			// bxVol = CM::get()->find_config_as_box("BOX_CUBE");//Box(-5, 0, -5, 10, 10, 10);
			bxVol = Box(-25, -25, -25, 50, 50, 50);
			break;
		case MDL_TEST_PYRAMID:
			bxVol = CM::get()->find_config_as_box("BOX_PYRAMID");//Box(-20, 0, -20, 40, 40, 40);
			//pm->setColBox(CB_LARGE);
			break;
		case MDL_TEST_PLANE:
#define WALL_WIDTH 150
			bxVol = Box(-WALL_WIDTH / 2, 0, -WALL_WIDTH / 2,
					WALL_WIDTH, 10, WALL_WIDTH);
			//pm->setColBox(CB_FLAT);
			break;
		default:
			bxVol = Box();
			break;
	}
	pm = new PhysicsModel(pos, rot, 50);
	testBoxIndex = pm->addBox(bxVol);
	t = 0;
}


TestSObj::~TestSObj(void) {
}

bool TestSObj::update() {
#define MOVE_AMT 1
#define DIV 25
	/*
	 * North = +Z (away from player start)
	 * East  = +X (right of player start)
	 * South = -Z (towards player start)
	 * West  = -X (left of player start)
	 */
	switch(dir) {
	case TEST_NORTH:
		pm->applyForce(Vec3f(0, 0, MOVE_AMT * sin((float)t / DIV)));
		break;
	case TEST_EAST:
		pm->applyForce(Vec3f(MOVE_AMT * sin((float)t / DIV), 0, 0));
		break;
	case TEST_SOUTH:
		pm->applyForce(Vec3f(0, 0, -MOVE_AMT * sin((float)t / DIV)));
		break;
	case TEST_WEST:
		pm->applyForce(Vec3f(-MOVE_AMT * sin((float)t / DIV), 0, 0));
		break;
	default:
		break;
	}
	++t;

	// update box randomly
	//bxVol.w++;
	//bxVol.l++;
	//bxVol.x--;
	//bxVol.y--;
	//pm->updateBox(testBoxIndex, bxVol);



	return false;
}

int TestSObj::serialize(char * buf) {
	ObjectState *state = (ObjectState*)buf;
	state->modelNum = modelNum;

	if (SOM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(buf + sizeof(ObjectState));

		vector<Box> objBoxes = pm->colBoxes;

		collState->totalBoxes = min(objBoxes.size(), maxBoxes);

		for (int i=0; i<collState->totalBoxes; i++)
		{
			collState->boxes[i] = objBoxes[i] + pm->ref->getPos(); // copying applyPhysics
		}

		return pm->ref->serialize(buf + sizeof(ObjectState) + sizeof(CollisionState)) + sizeof(ObjectState) + sizeof(CollisionState);
	}
	else
	{
		return pm->ref->serialize(buf + sizeof(ObjectState)) + sizeof(ObjectState);
	}
}
