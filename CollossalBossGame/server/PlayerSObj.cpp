#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "WallSObj.h"
#include "defs.h"

PlayerSObj::PlayerSObj(uint id) : ServerObject(id) {
	// Configuration options
	jumpDist = CM::get()->find_config_as_float("JUMP_DIST");
	movDamp = CM::get()->find_config_as_int("MOV_DAMP");

	Point_t pos = Point_t(0, 5, 10);
	Box bxVol = CM::get()->find_config_as_box("BOX_CUBE");//Box(-10, 0, -10, 20, 20, 20);

	DC::get()->print("Created new PlayerSObj %d\n", id);
	//pm = new PhysicsModel(Point_t(-50,0,150), Rot_t(), 5);
	pm = new PhysicsModel(pos, Rot_t(), CM::get()->find_config_as_float("PLAYER_MASS"), bxVol);
	lastCollision = pos;
	this->health = CM::get()->find_config_as_int("INIT_HEALTH");
	// Initialize input status
	istat.attack = false;
	istat.jump = false;
	istat.quit = false;
	istat.specialPower = false;
	istat.rotAngle = 0.0;
	istat.rotHoriz = 0.0;
	istat.rotVert = 0.0;
	istat.rightDist = 0.0;
	istat.forwardDist = 0.0;
}


PlayerSObj::~PlayerSObj(void) {
	delete pm;
}

bool PlayerSObj::update() {
	float yDist = 0.f;
	if (istat.quit) {
		// todo Send Client quit event
		return true; // delete me!
	}
	
	if(this->health > 0)
	{
		if (istat.attack) {
			// Determine attack logic here
		}
		// Jumping can happen in two cases
		// 1. Collisions
		// 2. In the air
		// This handles in the air, collisions
		// are handled in OnCollision()
		if (istat.jump && getFlag(IS_FALLING))
		{
			// start a counter and keep it going somewhow.
			jumping = true;
		}

		if (jumping) jumpCounter++;
		else jumpCounter = 0;
		/*if (istat.jump) {
			if(!getFlag(IS_FALLING)) yDist = jumpDist;
			else
			{
				jumpCounter++;
			}
		// Making it two discrete jumps
		jumping = true;
		}
		else
		{
			jumping = false;
		}*/
		if (istat.specialPower) {
			// Determine special power logic here
			pm->ref->setPos(Vec3f()); // your special power is to return to the origin
		}
	
		Rot_t rt = pm->ref->getRot();
		float yaw = rt.y + istat.rotHoriz,
			  pitch = rt.x + istat.rotVert;
		if (yaw > M_TAU || yaw < -M_TAU) yaw = 0;
		if (pitch > M_TAU || pitch < -M_TAU) pitch = 0;
		pm->ref->setRot(Rot_t(0, yaw, 0));
	
		int divBy = movDamp;
		float rawRight = istat.rightDist / divBy;
		float rawForward = istat.forwardDist / divBy;
		float computedRight = ((rawForward * sin(yaw)) + (rawRight * sin(yaw + M_PI / 2.f)));
		float computedForward = ((rawForward * cos(yaw)) + (rawRight * cos(yaw + M_PI / 2.f)));
		pm->applyForce(Vec3f(computedRight, yDist, computedForward));	
	}
	return false;
}

int PlayerSObj::serialize(char * buf) {
	PlayerState *state = (PlayerState*)buf;
	state->modelNum = MDL_0;
	state->health = health;
	return pm->ref->serialize(buf + sizeof(PlayerState)) + sizeof(PlayerState);
}

void PlayerSObj::deserialize(char* newInput)
{
	inputstatus* newStatus = reinterpret_cast<inputstatus*>(newInput);
	istat = *newStatus;
}

void PlayerSObj::onCollision(ServerObject *obj) {
	DC::get()->print("Collided with obj %d\n", obj->getId());
	if(obj->getFlag(IS_HARMFUL))
		this->health--;
	if(obj->getFlag(IS_HEALTHY))
		this->health++;
	if(this->health < 0) health = 0;
	if(this->health > 100) health = 100;

	// If I started jumping a little bit ago, that's a jump
	if(istat.jump)//jumpCounter > 0 && jumpCounter < 20) // button mashing problem
	{
		// surface bouncing
		if(obj->getFlag(IS_WALL))
		{
			// Get the normal from the surface
			WallSObj *wall  = reinterpret_cast<WallSObj *>(obj);
			float bounceDamp = 0.05f;

			Vec3f incident = pm->ref->getPos() - lastCollision;
			Vec3f normal = wall->getNormal();

			// incident is zero, so we just jump
			if ((incident.x < .01 && incident.x > -.01)
				|| (incident.y < .01 && incident.y > -.01)
				|| (incident.z < .01 && incident.z > -.01))
			{
				Vec3f up = Vec3f(0, 1, 0);
				Vec3f force = up + normal;
				pm->applyForce(force*jumpDist);
			}
			// we have incident! so we bounce
			else
			{
				// http://www.3dkingdoms.com/weekly/weekly.php?a=2
				// optimize: *= ^= better!
				pm->vel = (normal * (((incident ^ normal) * -2.f )) + incident) * bounceDamp;
			}
		}
		// object bouncing
		else
		{
			// get the normal we collided against
			Vec3f up = Vec3f(0, 1, 0);
			pm->applyForce(up*jumpDist);
		}
	}

	/*if(obj->getFlag(IS_WALL) && jumpCounter > 0 && jumpCounter < 20) //&& istat.jump)
	{
		//jump!
		WallSObj *wall  = reinterpret_cast<WallSObj *>(obj);
		float bounceDamp = 0.05f;

		Vec3f incident = pm->ref->getPos() - lastCollision;
		Vec3f normal = wall->getNormal();
		Vec3f reflected = incident - (((normal - incident) * normal) * 2.f);
		// http://www.3dkingdoms.com/weekly/weekly.php?a=2

		pm->vel = (normal * (((incident ^ normal) * -2.f )) + incident) * bounceDamp;
	}*/


	// Set last collision pos
	lastCollision = pm->ref->getPos();
	jumping = false;
}
