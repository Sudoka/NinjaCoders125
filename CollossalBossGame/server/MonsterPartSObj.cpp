#include "MonsterPartSObj.h"
#include "ConfigurationManager.h"
#include "ServerObjectManager.h"

MonsterPartSObj::MonsterPartSObj(uint id, Model modelNum, Point_t pos, Quat_t rot, MonsterSObj* master) : ServerObject(id)
{
	this->overlord = master;
	overlord->addPart(this);
	this->modelNum = modelNum;

	this->health = CM::get()->find_config_as_int("INIT_HEALTH");

	pm = new PhysicsModel(pos, rot, 50*CM::get()->find_config_as_float("PLAYER_MASS"));

	this->setFlag(IS_STATIC, 1);

	isFogging = false; 
}


MonsterPartSObj::~MonsterPartSObj(void)
{
	delete pm;
}

int MonsterPartSObj::serialize(char * buf) {
	MonsterPartState *state = (MonsterPartState*)buf;
	state->modelNum = this->modelNum;
	state->animationState = this->modelAnimationState;
	state->fog = this->isFogging;
	state->animationFrame = -1;

	if (SOM::get()->collisionMode)
	{
		CollisionState *collState = (CollisionState*)(buf + sizeof(MonsterPartState));
		vector<Box> objBoxes = pm->colBoxes;
		collState->totalBoxes = min(objBoxes.size(), maxBoxes);
		
		for (int i=0; i<collState->totalBoxes; i++)
		{
			collState->boxes[i] = objBoxes[i] + pm->ref->getPos(); // copying applyPhysics
		}
		return pm->ref->serialize(buf + sizeof(MonsterPartState) + sizeof(CollisionState)) + sizeof(MonsterPartState) + sizeof(CollisionState);
	}
	else
	{
		return pm->ref->serialize(buf + sizeof(MonsterPartState)) + sizeof(MonsterPartState);
	}
}