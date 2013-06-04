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
	////////////////////////// TENTACLES //////////////////////////

	// Floor
	int adjustment = 60;
	availTentaclePlacementsW.push_back(Frame(Vec3f(-265, 45+adjustment, 120), Quat_t(Vec3f(1, 0, 0), M_PI/2)));
	availTentaclePlacementsE.push_back(Frame(Vec3f(370, 45+adjustment, -220), Quat_t(Vec3f(1, 0, 0), M_PI/2)));
	availTentaclePlacementsE.push_back(Frame(Vec3f(75, 45+adjustment, -20), Quat_t(Vec3f(1, 0, 0), M_PI/2)));

	
	// East Wall (non-window)
	availTentaclePlacementsE.push_back(Frame(Vec3f(585.5-adjustment, 85, 75), Quat_t(Vec3f(0, 1, 0), M_PI/2)));
	availTentaclePlacementsE.push_back(Frame(Vec3f(585.5-adjustment, 220, -160), Quat_t(Vec3f(0, 1, 0), M_PI/2)));

	// North Wall
	availTentaclePlacementsE.push_back(Frame(Vec3f(140.25, 147.5, -300+adjustment), Quat_t(Vec3f(1, 0, 0), M_PI)));
	availTentaclePlacementsE.push_back(Frame(Vec3f(106.25, 50, -300+adjustment), Quat_t(Vec3f(1, 0, 0), M_PI)));
	availTentaclePlacementsW.push_back(Frame(Vec3f(-106.25, 50, -300+adjustment), Quat_t(Vec3f(1, 0, 0), M_PI)));
	availTentaclePlacementsE.push_back(Frame(Vec3f(318.75, 147.5, -300+adjustment), Quat_t(Vec3f(1, 0, 0), M_PI)));
	availTentaclePlacementsE.push_back(Frame(Vec3f(532.5, 250.15, -300+adjustment), Quat_t(Vec3f(1, 0, 0), M_PI)));
	availTentaclePlacementsW.push_back(Frame(Vec3f(-73.25, 245.15, -300+adjustment), Quat_t(Vec3f(1, 0, 0), M_PI)));
	availTentaclePlacementsW.push_back(Frame(Vec3f(-570.5, 258.15, -300+adjustment), Quat_t(Vec3f(1, 0, 0), M_PI)));
	availTentaclePlacementsW.push_back(Frame(Vec3f(-553.5, 147.5, -300+adjustment), Quat_t(Vec3f(1, 0, 0), M_PI)));
	
	// South Wall
	
	availTentaclePlacementsE.push_back(Frame(Vec3f(553.5, 147.5, 300-adjustment), Quat_t(Vec3f(0, 0, 0), 0)));
	availTentaclePlacementsE.push_back(Frame(Vec3f(570.5, 258.15, 300-adjustment), Quat_t(Vec3f(0, 0, 0), 0)));
	availTentaclePlacementsE.push_back(Frame(Vec3f(73.25, 245.15, 300-adjustment), Quat_t(Vec3f(0, 0, 0), 0)));
	availTentaclePlacementsW.push_back(Frame(Vec3f(-532.5, 250.15, 300-adjustment), Quat_t(Vec3f(0, 0, 0), 0)));
	availTentaclePlacementsW.push_back(Frame(Vec3f(-318.75, 147.5, 300-adjustment), Quat_t(Vec3f(0, 0, 0), 0)));
	availTentaclePlacementsE.push_back(Frame(Vec3f(106.25, 50, 300-adjustment), Quat_t(Vec3f(0, 0, 0), 0)));
	availTentaclePlacementsW.push_back(Frame(Vec3f(-106.25, 50, 300-adjustment), Quat_t(Vec3f(0, 0, 0), 0)));
	availTentaclePlacementsW.push_back(Frame(Vec3f(-140.25, 147.5, 300-adjustment), Quat_t(Vec3f(0, 0, 0), 0)));

	////////////////////////// HEADS //////////////////////////
	// Floor
	availHeadPlacementsW.push_back(Frame(Vec3f(-265.f, -20.f, 140.f), Quat_t(Vec3f(1.f, 0, 0), (float)3*M_PI/10)));
	availHeadPlacementsE.push_back(Frame(Vec3f(380.f, -20.f, -240.f), Quat_t(Vec3f(0, 1.f, 0), (float)M_PI) * Quat_t(Vec3f(1.f, 0, 0), (float)3*M_PI/10)));
	availHeadPlacementsE.push_back(Frame(Vec3f(75.f, -20.f, 10.f), Quat_t(Vec3f(0, 1.f, 0), (float)M_PI/4) * Quat_t(Vec3f(1.f, 0, 0), (float)3*M_PI/10)));

	// East Wall (non-window)
	availHeadPlacementsE.push_back(Frame(Vec3f(640.5f, 95.f, 75.f), Quat_t(Vec3f(0, 1.f, 0), (float)M_PI/2)));
	availHeadPlacementsE.push_back(Frame(Vec3f(640.5f, 225.f, -160.f), Quat_t(Vec3f(0, 1.f, 0), (float)M_PI/2)));

	// North Wall
	availHeadPlacementsE.push_back(Frame(Vec3f(525.5f, 147.5f, -360.f),  Quat_t(Vec3f(0, 0, 1.f), (float)M_PI) * Quat_t(Vec3f(1.f, 0, 0), (float)M_PI)));
	//availHeadPlacements.push_back(Frame(Vec3f(-505.5f, 258.15f, 300.f),  Quat_t(Vec3f(1.f, 0, 0), (float)M_PI)));
	//availHeadPlacements.push_back(Frame(Vec3f(-135.25f, 260.15f, 300.f),  Quat_t(Vec3f(1.f, 0, 0), (float)M_PI)));
	//availHeadPlacements.push_back(Frame(Vec3f(532.5f, 250.15f, 300.f),  Quat_t(Vec3f(1.f, 0, 0), (float)M_PI)));
	availHeadPlacementsW.push_back(Frame(Vec3f(-318.75f, 147.5f, -360.f),   Quat_t(Vec3f(0, 0, 1.f), (float)M_PI) * Quat_t(Vec3f(1.f, 0, 0), (float)M_PI)));
	availHeadPlacementsE.push_back(Frame(Vec3f(106.25f, 50.f, -360.f),   Quat_t(Vec3f(0, 0, 1.f), (float)M_PI) * Quat_t(Vec3f(1.f, 0, 0), (float)M_PI)));
	availHeadPlacementsW.push_back(Frame(Vec3f(-106.25f, 50.f, -360.f),   Quat_t(Vec3f(0, 0, 1.f), (float)M_PI) * Quat_t(Vec3f(1.f, 0, 0), (float)M_PI)));
	availHeadPlacementsW.push_back(Frame(Vec3f(-140.25f, 157.5f, -360.f),   Quat_t(Vec3f(0, 0, 1.f), (float)M_PI) * Quat_t(Vec3f(1.f, 0, 0), (float)M_PI)));


	// South Wall
	availHeadPlacementsW.push_back(Frame(Vec3f(-525.5f, 147.5f, 360.f), Quat_t()));
	//availHeadPlacements.push_back(Frame(Vec3f(-505.5f, 258.15f, 300.f), Quat_t()));
	//availHeadPlacements.push_back(Frame(Vec3f(-135.25f, 260.15f, 300.f), Quat_t()));
	//availHeadPlacements.push_back(Frame(Vec3f(532.5f, 250.15f, 300.f), Quat_t()));
	availHeadPlacementsE.push_back(Frame(Vec3f(318.75f, 147.5f, 360.f), Quat_t()));
	availHeadPlacementsW.push_back(Frame(Vec3f(-106.25f, 50.f, 360.f), Quat_t()));
	availHeadPlacementsE.push_back(Frame(Vec3f(106.25f, 50.f, 360.f), Quat_t()));
	availHeadPlacementsE.push_back(Frame(Vec3f(140.25f, 157.5f, 360.f), Quat_t()));
}

void MonsterSObj::removePart(MonsterPartSObj* t)
{ 
	parts.erase(t); 
	Frame* fr = t->getPhysicsModel()->ref; 
	if (t->getType() == OBJ_TENTACLE) {
		if (fr->getPos().x > 0)	availTentaclePlacementsE.push_back(*fr);
		else	availTentaclePlacementsW.push_back(*fr);
	}
	else if (t->getType() == OBJ_HEAD) {
		if (fr->getPos().x > 0) availHeadPlacementsE.push_back(*fr);
		else  availHeadPlacementsW.push_back(*fr);
	}
}

/**
 * This is what tentacles call when they want to change their position
 */
Frame MonsterSObj::updatePosition(Frame oldPos, ObjectType childType) {
	Frame result;

	vector<Frame>* choosePositionFrom;
	vector<Frame>* addPositionTo = NULL; // set to non-null if oldPos comes from different list

	DIRECTION gravity = PE::get()->getGravDir();

	// Pick a list depending on what you are
	if (childType == OBJ_TENTACLE)
	{
		// If gravity is east or west, they should go towards you, otherwise random
		if (gravity == EAST) choosePositionFrom = &availTentaclePlacementsE;
		else if (gravity == WEST) choosePositionFrom = &availTentaclePlacementsW;
		else {
			if (rand()%2) choosePositionFrom = &availTentaclePlacementsE;
			else choosePositionFrom = &availTentaclePlacementsW;
		}

		// check if we should add the old position to a different list
		if (choosePositionFrom == &availTentaclePlacementsE && oldPos.getPos().x <= 0) {
			addPositionTo = &availTentaclePlacementsW;
		}
		else if (choosePositionFrom == &availTentaclePlacementsW && oldPos.getPos().x > 0) {
			addPositionTo = &availTentaclePlacementsE;
		}
	}
	else if (childType == OBJ_HEAD)
	{
		// If gravity is east or west, they should go towards you, otherwise random
		if (gravity == EAST) choosePositionFrom = &availHeadPlacementsE;
		else if (gravity == WEST) choosePositionFrom = &availHeadPlacementsW;
		else {
			if (rand()%2) choosePositionFrom = &availHeadPlacementsE;
			else choosePositionFrom = &availHeadPlacementsW;
		}
		
		// check if we should add the old position to a different list
		if (choosePositionFrom == &availHeadPlacementsE && oldPos.getPos().x <= 0) {
			addPositionTo = &availHeadPlacementsW;
		}
		else if (choosePositionFrom == &availHeadPlacementsW && oldPos.getPos().x > 0) {
			addPositionTo = &availHeadPlacementsE;
		}
	}

	// Make sure we actually have positions
	assert(choosePositionFrom->size() > 0 && "You ran out of positions for your tentacles!");

	// Now pick one at random
	int index = rand() % choosePositionFrom->size();
	result = choosePositionFrom->at(index);

	// Now do the switch if needed
	if (addPositionTo != NULL)
	{
		// First, fix the list we got a new position from
		choosePositionFrom->at(index) = choosePositionFrom->back();
		choosePositionFrom->pop_back();

		// Now, just add the old position to the right list
		addPositionTo->push_back(oldPos);
	}
	// Otherwise, this is a simple swap like before
	else
	{
		choosePositionFrom->at(index) = oldPos;
	}

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
		//assert(availTentaclePlacements.size() >= numParts && availHeadPlacements.size() >= numParts && "You ran out of positions for your tentacles!");

		// shuffle your positions
		std::random_shuffle(availTentaclePlacementsE.begin(), availTentaclePlacementsE.end());
		std::random_shuffle(availTentaclePlacementsW.begin(), availTentaclePlacementsW.end());
		std::random_shuffle(availHeadPlacementsE.begin(), availHeadPlacementsE.end());
		std::random_shuffle(availHeadPlacementsW.begin(), availHeadPlacementsW.end());

		// Initialize your parts (tentacles, or hydras, or mixed, or whatever)
		for (uint i=0; i<numParts; i++)
		{
			Frame currPlace; 

			MonsterPartSObj * newPart;
			switch (phase)
			{
			case 0:
				// pick the random position TODO THIS SOULD BE TENTACLE PLACEMENTS!
				//currPlace = availTentaclePlacements.back();
				//availTentaclePlacements.pop_back();
	
				//newPart = new TentacleSObj(SOM::get()->genId(), (Model)(i + MDL_TENTACLE_1), currPlace.getPos(), currPlace.getRot(), this);

				if (rand()%2) { currPlace = availHeadPlacementsE.back(); availHeadPlacementsE.pop_back(); }
				else { currPlace = availHeadPlacementsW.back(); availHeadPlacementsW.pop_back(); }

				newPart = new HeadSObj(SOM::get()->genId(), (Model)(i + MDL_HEAD_1), currPlace.getPos(), currPlace.getRot(), this);

				break;
			case 1:
				// pick the random position
				if (rand()%2) { currPlace = availTentaclePlacementsE.back(); availTentaclePlacementsE.pop_back(); }
				else { currPlace = availTentaclePlacementsW.back(); availTentaclePlacementsW.pop_back(); } 

				newPart = new HeadSObj(SOM::get()->genId(), (Model)(i + MDL_HEAD_1), currPlace.getPos(), currPlace.getRot(), this);
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
