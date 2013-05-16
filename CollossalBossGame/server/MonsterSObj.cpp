#include "MonsterSObj.h"
#include "ConfigurationManager.h"
#include "ServerObjectManager.h"
#include "defs.h"
#include "PlayerSObj.h"
#include "GameServer.h"
#include <time.h>

MonsterSObj::MonsterSObj(uint id, uint numParts) : ServerObject(id)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new MonsterObj %d\n", id);
	this->health = 0;
	// todo make null make sure it works
	pm = new PhysicsModel(Point_t(), Quat_t(), CM::get()->find_config_as_float("PLAYER_MASS"));
	this->setFlag(IS_STATIC, 1);
	
	this->availablePlacements = CM::get()->find_config_as_places("TENTACLE_POSITIONS");

	for(map<Point_t,Quat_t>::iterator it = availablePlacements.begin(); it != availablePlacements.end(); ++it) {
	  placements.push_back(it->first);
	}

	this->numParts = numParts;
	phase = -1;
	GameServer::get()->event_monster_spawn();

	srand((uint)time(NULL)); // initialize our random number generator

}

MonsterSObj::~MonsterSObj(void)
{
	delete pm;
}

void MonsterSObj::removeTentacle(TentacleSObj* t)
{ 
	tentacles.erase(t); 
	Frame* fr = t->getPhysicsModel()->ref; 
	availablePlacements[fr->getPos()] = fr->getRot(); 
}

/* update()
 * Whenever the tentacle updates, it figures what state it's in (aiming, sweeping, smashing, or idling).
 * If it's idling and someone's nearby, then it can start aiming when we get that figured out. 
 * If idling, someone is not near by, we can randomly start sweeping.
 * Once we get aiming & smashing a single player, if aiming and we've waited enough time, then we smash
 * If attacking or sweeping, we continue.
 *
 * Author: Bryan, Haro, Suman
 */
bool MonsterSObj::update() {
	int numTentacles = tentacles.size();
	health = 0;
	if(numTentacles > 0) {
		for (set<TentacleSObj*>::iterator it = tentacles.begin();
			it != tentacles.end();
			++it)
			health += (*it)->getHealth();
		health /= numParts;
	}

	if (numTentacles == 0) {
		phase++;

		// Initialize your parts (tentacles, or hydras, or mixed, or whatever)
		for (uint i=0; i<numParts; i++)
		{
			// pick the random position
			map<Point_t, Quat_t>::iterator it;
			do {
				Point_t randPoint = placements[rand() % placements.size()];
				it = availablePlacements.find(randPoint);
			} while (it == availablePlacements.end());
			pair<Point_t, Quat_t> currPlace = *it;
			availablePlacements.erase(it);

			TentacleSObj * newTentacle;
			switch (phase)
			{
			case 0:
				newTentacle = new TentacleSObj(SOM::get()->genId(), (Model)i, currPlace.first, currPlace.second, this);
				break;
			case 1:
				// todo heads
				newTentacle = new TentacleSObj(SOM::get()->genId(), (Model)i, currPlace.first, currPlace.second, this);
				break;
			default: // you beat all the phases!
				GameServer::get()->event_monster_death();
				return true; // I died!
				// DO NOTHING MORE
				// DONT YOU DARE
			}

			this->addTentacle(newTentacle);
			SOM::get()->add(newTentacle);
		}
	}

	// Decide if we want our tentacles to fog
	const int fogProb = 1; // todo config maybe
	int x = rand() % 1000;
	bool fogging = x < fogProb;
	for (set<TentacleSObj*>::iterator it = tentacles.begin();
			it != tentacles.end();
			++it) {
				(*it)->setFogging(fogging);
	}

	// UNSURE for now!
	// Monster AI
	/*
	if (!stateCounter--)
	{
		distribution = std::uniform_int_distribution<int> (CYCLE*9, CYCLE*50);
		stateCounter = distribution(generator);
		// todo config field DC::get()->print("stateCounter %d\n", stateCounter);
	}
	switch (state) 
	{
	case IDLE:

		break;
	case SWEEP:
		state = IDLE;
		break;
	default:
		break;
	}
	*/
	return false;
}

int MonsterSObj::serialize(char * buf) {
	MonsterState *state = (MonsterState*)buf;
	state->health = health;
	return this->getPhysicsModel()->ref->serialize(buf + sizeof(MonsterState)) + sizeof(MonsterState);
}

void MonsterSObj::onCollision(ServerObject *obj, const Vec3f &collisionNormal) {
	// should only collide on tentacle this is a container class
}