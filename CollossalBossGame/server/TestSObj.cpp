#include "TestSObj.h"
#include "ServerObjectManager.h"
#include <math.h>
#include "ConfigurationManager.h"

TestSObj::TestSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, int dir) : ServerObject(id) {
	if(SOM::get()->debugFlag) DC::get()->print("Created new TestSObj %d\n", id);
	setFlag(IS_FALLING,1);
	int mass = 100;
	this->dir = dir;
	this->modelNum = modelNum;
	switch (modelNum) {
		case MDL_TEST_CRATE:
			mass = (rand() % 20) + 10;
		case MDL_TEST_BOX:
			// bxVol = CM::get()->find_config_as_box("BOX_CUBE");//Box(-5, 0, -5, 10, 10, 10);
			bxVol = Box( -10, -10, -10, 20, 20, 20);
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

	pm = new PhysicsModel(pos, rot, (float)mass);
	testBoxIndex = getCollisionModel()->add(new AabbElement(bxVol));
	t = 0;
}


TestSObj::~TestSObj(void) {
	delete pm;
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
	case TEST_STILL:
		setFlag(IS_FALLING,true);
		break;
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
		pm->applyForce(Vec3f(0, -MOVE_AMT * sin((float)t / DIV), 0));
		break;
	}
	++t;
	pm->frictCoeff = 1.3f;
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
