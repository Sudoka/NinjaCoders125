#include "ScientistCObj.h"



ScientistCObj::ScientistCObj(uint id, char *data) : PlayerCObj(id, data)
{
	
}


ScientistCObj::~ScientistCObj(void)
{
}

int ScientistCObj::getTypeInt()
{
	return 2;
}