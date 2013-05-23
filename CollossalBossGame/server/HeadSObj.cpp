#include "HeadSObj.h"
#include "ServerObjectManager.h"
#include "ConfigurationManager.h"
#include "RageSObj.h"
#include "FireBallSObj.h"

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
	// First, create our bullet
	if (stateCounter == 0)
	{
		Vec3f mpos = this->getPhysicsModel()->ref->getPos();
		mpos.z += 200;
		float distance = 10000.f; // MAX DISTANCE FOR TARGETING
		int playerid = -1;
		vector<ServerObject *> vso;
		Vec3f minPos;
		SOM::get()->findObjects(OBJ_PLAYER, &vso);
		for (int i = 0; i < vso.size(); i++) {
			Vec3f ppos = vso[i]->getPhysicsModel()->ref->getPos();
			float tdist = fabs(magnitude(ppos-mpos));
			if(tdist < distance) {
				distance = tdist;
				playerid = vso[i]->getId();
				minPos = ppos;
			}
		}
		if(playerid > -1) {
			// normalize
			Vec3f motion = minPos - mpos;
			motion.normalize();
			Vec3f offset = motion * 35*1.5;


			float diameter = 45; // todo define? config?
			int damage = 10; // todo config
			//Vec3f offset = rotate(Vec3f(0, upforce * diameter * sqrt(2.0f), forwardforce * diameter * sqrt(2.0f)), pm->ref->getRot());
			Vec3f position = mpos + offset;
			FireBallSObj * fbso = new FireBallSObj(SOM::get()->genId(), (Model)-1, position, motion, damage, (int)diameter);
			SOM::get()->add(fbso);
		}
	}

	// this is random, for now
	currStateDone = (stateCounter == 30);

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