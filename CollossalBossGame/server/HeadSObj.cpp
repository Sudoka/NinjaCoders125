#include "HeadSObj.h"
#include "ServerObjectManager.h"
#include "ConfigurationManager.h"
#include "RageSObj.h"

HeadSObj::HeadSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master) :
											MonsterPartSObj(id, modelNum, pos, rot, master)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new HeadSObj %d\n", id);

	/////////////// Collision Boxes /////////////////
	idleBoxes[0] = CM::get()->find_config_as_box("BOX_HEAD_BASE"); 
	idleBoxes[1] = CM::get()->find_config_as_box("BOX_HEAD_MID");
	idleBoxes[2] = CM::get()->find_config_as_box("BOX_HEAD_TIP");

	CollisionModel *cm = getCollisionModel();

	for (int i=0; i<3; i++) {
		assert((cm->add(new AabbElement(idleBoxes[i])) == i) && "Your physics model is out of sync with the rest of the world...");
	}

	modelAnimationState = M_IDLE;
}


HeadSObj::~HeadSObj(void)
{
}

void HeadSObj::idle() {

}

void HeadSObj::probe() {

}

void HeadSObj::attack() {

}

void HeadSObj::combo() {

}

void HeadSObj::spike() {

}

// FOR NOW this is the same as in the tentacle
void HeadSObj::rage() {
	//modelAnimationState = M_RAGE;

	// First, we create the wave object
	if (stateCounter == 0) {
		Vec4f axis = this->getPhysicsModel()->ref->getRot();
		Vec3f changePos = Vec3f(0,0,-120);
		changePos = axis.rotateToThisAxis(changePos);
		SOM::get()->add(new RageSObj(SOM::get()->genId(), pm->ref->getPos() + changePos));
	}

	// for now, keep our initial idle collision boxes
	Box origBase = idleBoxes[0];
	Box origMiddle = idleBoxes[1];
	Box origTip = idleBoxes[2];

	//get the actual axis
	Vec4f axis = this->getPhysicsModel()->ref->getRot();

	origBase.setPos(axis.rotateToThisAxis(origBase.getPos()));
	origBase.setSize(axis.rotateToThisAxis(origBase.getSize()));

	origMiddle.setPos(axis.rotateToThisAxis(origMiddle.getPos()));
	origMiddle.setSize(axis.rotateToThisAxis(origMiddle.getSize()));

	origTip.setPos(axis.rotateToThisAxis(origTip.getPos()));
	origTip.setSize(axis.rotateToThisAxis(origTip.getSize()));

	CollisionModel *cm = getCollisionModel();
	((AabbElement*)cm->get(0))->bx = *(origBase.fix());
	((AabbElement*)cm->get(1))->bx = *(origMiddle.fix());
	((AabbElement*)cm->get(2))->bx = *(origTip.fix());

	// when the object dies we're done raging
	currStateDone = stateCounter >= RageSObj::lifetime;
}