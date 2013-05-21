#pragma once
#include "ServerObject.h"
#include "MonsterSObj.h"

class MonsterPartSObj :
	public ServerObject
{
public:
	MonsterPartSObj(uint id, MonsterSObj* master);
	virtual ~MonsterPartSObj(void);
};

