#include "CyborgCObj.h"


CyborgCObj::CyborgCObj(uint id, char *data) : PlayerCObj(id, data)
{
	
}


CyborgCObj::~CyborgCObj(void)
{
}

int CyborgCObj::getTypeInt()
{
	return 0;
}