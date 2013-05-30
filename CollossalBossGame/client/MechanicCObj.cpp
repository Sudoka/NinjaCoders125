#include "MechanicCObj.h"


MechanicCObj::MechanicCObj(uint id, char *data) : PlayerCObj(id, data)
{
	
}


MechanicCObj::~MechanicCObj(void)
{
}

int MechanicCObj::getTypeInt()
{
	return 1;
}