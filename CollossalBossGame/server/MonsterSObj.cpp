#include "MonsterSObj.h"
#include "ConfigurationManager.h"
#include "ServerObjectManager.h"
#include "defs.h"
#include "PlayerSObj.h"
#include "GameServer.h"
#include <time.h>
#include <algorithm>

#include "MonsterPartSObj.h"
#include "TentacleSObj.h"
#include "HeadSObj.h"


MonsterSObj::MonsterSObj(uint id, uint numParts) : ServerObject(id)
{
	if(SOM::get()->debugFlag) DC::get()->print("Created new MonsterObj %d\n", id);
	this->health = 0;
	// todo make null make sure it works
	//pm = new PhysicsModel(Point_t(), Quat_t(), CM::get()->find_config_as_float("PLAYER_MASS"));
	this->setFlag(IS_STATIC, 1);
	
	setupAvailablePlacements();
	//this->availablePlacements = CM::get()->find_config_as_places("TENTACLE_POSITIONS");

	/*for(map<Point_t,Quat_t>::iterator it = availablePlacements.begin(); it != availablePlacements.end(); ++it) {
	  placements.push_back(it->first);
	}*/

	this->numParts = numParts;
	phase = -1;
	GameServer::get()->event_monster_spawn();

	srand((uint)time(NULL)); // initialize our random number generator

}

MonsterSObj::~MonsterSObj(void)
{
}

/** 
 * Programatically adds tentacle positions, as opposed
 * to doing it through the config, since the config 
 * can't handle long lists...
 */
void MonsterSObj::setupAvailablePlacements()
{
	// South Wall
	availablePlacements.push_back(Frame(Vec3f(553.5, 147.5, 300), Quat_t(Vec3f(0, 0, 0), 0)));
	availablePlacements.push_back(Frame(Vec3f(570.5, 258.15, 300), Quat_t(Vec3f(0, 0, 0), 0)));
	availablePlacements.push_back(Frame(Vec3f(73.25, 245.15, 300), Quat_t(Vec3f(0, 0, 0), 0)));
	availablePlacements.push_back(Frame(Vec3f(-532.5, 250.15, 300), Quat_t(Vec3f(0, 0, 0), 0)));
	availablePlacements.push_back(Frame(Vec3f(-318.75, 147.5, 300), Quat_t(Vec3f(0, 0, 0), 0)));
	availablePlacements.push_back(Frame(Vec3f(106.25, 50, 300), Quat_t(Vec3f(0, 0, 0), 0)));
	availablePlacements.push_back(Frame(Vec3f(-106.25, 50, 300), Quat_t(Vec3f(0, 0, 0), 0)));
	availablePlacements.push_back(Frame(Vec3f(-140.25, 147.5, 300), Quat_t(Vec3f(0, 0, 0), 0)));
}

void MonsterSObj::removePart(MonsterPartSObj* t)
{ 
	parts.erase(t); 
	Frame* fr = t->getPhysicsModel()->ref; 
	availablePlacements.push_back(*fr);
}

/**
 * This is what tentacles call when they want to change their position
 */
Frame MonsterSObj::updatePosition(Frame oldPos) {
	// Make sure we actually have positions
	assert(availablePlacements.size() > 0 && "You ran out of positions for your tentacles!");

	// Now pick one at random
	int index = rand() % availablePlacements.size();
	Frame result = availablePlacements[index];
	availablePlacements[index] = oldPos;

	return result;
	//vector<Point_t> possiblePlacements(placements);

	//map<Point_t, Quat_t>::iterator it;
	//do {
	//	Point_t randPoint = possiblePlacements[rand() % possiblePlacements.size()];
	//	it = availablePlacements.find(randPoint);
	//} while (it == availablePlacements.end());
	//pair<Point_t, Quat_t> currPlace = *it;
	//availablePlacements.erase(it);
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
	int numTentacles = parts.size();
	health = 0;
	if(numTentacles > 0) {
		for (set<MonsterPartSObj*>::iterator it = parts.begin();
			it != parts.end();
			++it)
			health += (*it)->getHealth();
		health /= numParts;
	}

	if (numTentacles == 0) {
		phase = (phase+1)%2;

		// Make sure we've got enough positions
		assert(availablePlacements.size() >= numParts && "You ran out of positions for your tentacles!");

		// shuffle your positions
		std::random_shuffle(availablePlacements.begin(), availablePlacements.end());

		// Initialize your parts (tentacles, or hydras, or mixed, or whatever)
		for (uint i=0; i<numParts; i++)
		{
			// pick the random position
			Frame currPlace = availablePlacements.back();
			availablePlacements.pop_back();

			MonsterPartSObj * newPart;
			switch (phase)
			{
			case 0:
				newPart = new TentacleSObj(SOM::get()->genId(), (Model)i, currPlace.getPos(), currPlace.getRot(), this);
				break;
			case 1:
				// todo heads different models 
				// todo animated head model...not-animated breaks the world xD
				newPart = new HeadSObj(SOM::get()->genId(), MDL_HEAD_1, currPlace.getPos(), currPlace.getRot(), this);
				break;
			default: // you beat all the phases!
				GameServer::get()->event_monster_death();
				return true; // I died!
				// DO NOTHING MORE
				// DONT YOU DARE
			}

			this->addPart(newPart);
			SOM::get()->add(newPart);
		}
	}

	// Decide if we want our tentacles to fog
	const int fogProb = 1; // todo config maybe
	int x = rand() % 1000;
	bool fogging = x < fogProb;
	for (set<MonsterPartSObj*>::iterator it = parts.begin();
			it != parts.end();
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
	return /*pm->ref->serialize(buf + sizeof(MonsterState)) + */ sizeof(MonsterState);
}

void MonsterSObj::onCollision(ServerObject *obj, const Vec3f &collisionNormal) {
	// should only collide on tentacle this is a container class
}
