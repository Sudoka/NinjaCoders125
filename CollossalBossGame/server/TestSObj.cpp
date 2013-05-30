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

	CollisionModel *cm = getCollisionModel();

	switch (modelNum) {
		case MDL_TEST_CRATE:
			mass = 20;
		case MDL_TEST_BOX:
			// bxVol = CM::get()->find_config_as_box("BOX_CUBE");//Box(-5, 0, -5, 10, 10, 10);
			testBoxIndex = cm->add(new AabbElement(Box( -10, -10, -10, 20, 20, 20)));
			break;
		case MDL_TEST_PYRAMID:
			//bxVol = CM::get()->find_config_as_box("BOX_PYRAMID");//Box(-20, 0, -20, 40, 40, 40);
			testBoxIndex = cm->add(new AabbElement(CM::get()->find_config_as_box("BOX_PYRAMID")));
			//pm->setColBox(CB_LARGE);
			break;
		case MDL_TEST_PLANE:
#define WALL_WIDTH 150
			testBoxIndex = cm->add(new AabbElement(Box(-WALL_WIDTH / 2, 0, -WALL_WIDTH / 2,
														WALL_WIDTH, 10, WALL_WIDTH)));
			//bxVol = Box(-WALL_WIDTH / 2, 0, -WALL_WIDTH / 2,
			//		WALL_WIDTH, 10, WALL_WIDTH);
			//pm->setColBox(CB_FLAT);
			break;
		case MDL_ELEVATOR:
			cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_ELEV_BASE").rotate(rot)->fix())));
			cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_ELEV_LEFT").rotate(rot)->fix())));
			cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_ELEV_RIGHT").rotate(rot)->fix())));
			cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_ELEV_BACK").rotate(rot)->fix())));
			cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_ELEV_TOP").rotate(rot)->fix())));
			cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_ELEV_FRONT_M_1").rotate(rot)->fix())));
			cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_ELEV_FRONT_M_2").rotate(rot)->fix())));
			cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_ELEV_FRONT_M_3").rotate(rot)->fix())));
			cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_ELEV_FRONT_M_4").rotate(rot)->fix())));
			cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_ELEV_FRONT_L").rotate(rot)->fix())));
			cm->add(new AabbElement(*(CM::get()->find_config_as_box("BOX_ELEV_FRONT_R").rotate(rot)->fix())));
		
			setFlag(IS_STATIC, true);
			break;
		default:
			//bxVol = Box();
			break;
	}

	pm = new PhysicsModel(pos, rot, (float)mass);
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
