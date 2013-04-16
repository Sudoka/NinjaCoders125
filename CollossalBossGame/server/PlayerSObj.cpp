#include "PlayerSObj.h"
#define M_PI 3.14159

PlayerSObj::PlayerSObj(uint id) : ServerObject(id) {
	DC::get()->print("Created new PlayerSObj %d\n", id);
	pm = new PhysicsModel(Point_t(-1000,0,700), Rot_t(), 5);

	// Initialize input status
	istat.attack = false;
	istat.jump = false;
	istat.quit = false;
	istat.specialPower = false;
	istat.rotAngle = 0.0;
	istat.xDist = 0.0;
	istat.yDist = 0.0;
}


PlayerSObj::~PlayerSObj(void) {
}

bool PlayerSObj::update() {
	if (istat.quit) {
		// Send Client quit event
		return true; // delete me!
	}
	if (istat.attack) {
		// Determine attack logic here
		DC::get()->print("SERVER KNOWS YOU'RE ATTACKING!!!                                               \r");
	}
	if (istat.jump) {
		DC::get()->print("SERVER KNOWS YOU'RE JUMPING!                                                   \r");
	}
	if (istat.specialPower) {
		DC::get()->print("SERVER KNOWS YOU'RE SPECIAL POWERING!!!!!                                      \r");
	}
	
	Rot_t rt = pm->ref->getRot();
	//rt.z += istat.rotHoriz;
	//rt.y += istat.rotVert;
	//pm->ref->setRot(rt);
	pm->ref->setRot(Rot_t(0, 0, istat.rotAngle));
	
	//pm->ref->setRot(Rot_t(0, istat.rotVert, istat.rotHoriz));
#define DIV 100
	//Point_t pos = pm->ref->getPos();
	//pm->ref->setPos(Point_t(pos.x + istat.xDist, pos.y - istat.yDist, 0));
	pm->applyForce(Vec3f(-istat.xDist / DIV, 0, -istat.yDist / DIV));
	

	return false;
}

int PlayerSObj::serialize(char * buf) {
	ObjectState *state = (ObjectState*)buf;
	state->modelNum = MDL_0;
	return pm->ref->serialize(buf + sizeof(ObjectState)) + sizeof(ObjectState);
}

void PlayerSObj::deserialize(char* newInput)
{
	inputstatus* newStatus = reinterpret_cast<inputstatus*>(newInput);
	istat.attack = newStatus->attack;
	istat.jump = newStatus->jump;
	istat.specialPower = newStatus->specialPower;
	istat.quit = newStatus->quit;
	istat.rotAngle = newStatus->rotAngle;
	istat.xDist = newStatus->xDist;
	istat.yDist = newStatus->yDist;
}