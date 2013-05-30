#include "ScientistSObj.h"
#include "BulletSObj.h"
#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "PhysicsEngine.h"
#include "ServerObjectManager.h"

ScientistSObj::ScientistSObj(uint id, uint clientId) : PlayerSObj(id, clientId, CHAR_CLASS_SCIENTIST)
{
	// Other re-initializations (things that don't depend on parameters, like config)
	this->initialize();
}


ScientistSObj::~ScientistSObj(void)
{
}

void ScientistSObj::initialize() {
	hso = NULL;
	this->currentTarget = NULL;
}

void ScientistSObj::actionAttack() {

}

void ScientistSObj::actionCharge(bool buttondown) {
	if (buttondown) {
		//1. If have just started to charge, then pick our object
		if (this->currentTarget == NULL) 
		{
			//1. get the closest player that's not the scientist
			bool playerFound = false;

			// Find a player with minimum distance to me
			vector<ServerObject *> players;
			SOM::get()->findObjects(OBJ_PLAYER, &players);

			float minDist = 20;
			float currDist;
			Vec3f difference, playerPos;

			int playerPosition = -1;
			int selfPosition = -1;
			int i = 0;

			Vec3f myPos = this->getPhysicsModel()->ref->getPos();

			for(vector<ServerObject *>::iterator it = players.begin(); it != players.end(); ++it) {
				playerPos = (*it)->getPhysicsModel()->ref->getPos();
				difference = playerPos - myPos;
				currDist = magnitude(difference);
				if (currDist < minDist && currDist != 0) {
					minDist = currDist;
					playerFound = true;
					playerPosition = i;
				} else if ( currDist == 0 ) {
					selfPosition = i;
				}
				i++;
			}

			//1b. if there aren't any players in range, get the scientist
			playerPosition = (playerPosition == -1) ? playerPosition : selfPosition;

			//1c. Store that info
			this->currentTarget = (PlayerSObj *) players.at(playerPosition);

			//2. set it to initial stats
			this->currentTarget->damage = (playerPosition == selfPosition) ? 7 : 4;
			this->currentTarget->scientistBuffCounter = 31;
		}

		//we now know we have the current target, so we can increment our damage and decrement our time
		this->currentTarget->damage += (this->currentTarget == this) ? 3 : 1;
		this->currentTarget->scientistBuffCounter -= 1;
	} else {
		//we are releasing it, so we're making it harmful for the appropriate amount of time.
		this->currentTarget->scientistBuffDecreasing = true;
		this->currentTarget = NULL;
	}
}
void ScientistSObj::clearAccessory() {
	
}