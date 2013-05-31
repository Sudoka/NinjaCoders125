#include "ScientistCObj.h"



ScientistCObj::ScientistCObj(uint id, char *data) : PlayerCObj(id, data+16)
{
	this->transformdelay = *(int *)data; data += 4;
	this->transformduration = *(int *)data; data += 4;
	this->transformtargetid = *(int *)data; data += 4;
	this->transformedclass = (CharacterClass)*(int *)data; data += 4;
}


ScientistCObj::~ScientistCObj(void)
{
}

int ScientistCObj::getTypeInt()
{
	return 2;
}

void ScientistCObj::deserialize(char * newState) {
	this->transformdelay = *(int *)newState; newState += 4;
	this->transformduration = *(int *)newState; newState += 4;
	this->transformtargetid = *(int *)newState; newState += 4;
	this->transformedclass = (CharacterClass)*(int *)newState; newState += 4;
	PlayerCObj::deserialize(newState);
}