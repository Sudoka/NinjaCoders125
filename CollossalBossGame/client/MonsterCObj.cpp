#include "MonsterCObj.h"
#include "ClientObjectManager.h"
#include "RenderEngine.h"
#include "AudioEngine.h"


MonsterCObj::MonsterCObj(uint id, char *data) : ClientObject(id, OBJ_MONSTER)
{
	if (COM::get()->debugFlag) DC::get()->print("Created new MonsterCObj %d\n", id);
	MonsterState *state = (MonsterState*)data;
	this->health = state->health;
	this->phase = state->phase;
	rm = new RenderModel(Point_t(),Quat_t(), (Model)-1);
	phaseOneStart = true;
	phaseTwoStart = true;
}

MonsterCObj::~MonsterCObj(void)
{
	// this is to be safe, and to fix a random bug we got 1/10,000 times
	if (RE::get() != NULL)	RE::get()->setMonsterHUDText("MONSTER!!", 0, phase);
	delete rm;
}

bool MonsterCObj::update() {
	if(phase == 0 && phaseOneStart) {
		// AE::get()->playAmbiance();
		phaseOneStart = false;
	}
	else if(phase == 1 && phaseTwoStart) {
		//AE::get()->stopAmbiance();
		// AE::get()->playMusic();
		phaseTwoStart = false;
	}
	RE::get()->setMonsterHUDText("MONSTER!!", health, phase);
	return false;
}

void MonsterCObj::deserialize(char* newState) {
	MonsterState *state = (MonsterState*)newState;
	this->health = state->health;
	this->phase = state->phase;
}
