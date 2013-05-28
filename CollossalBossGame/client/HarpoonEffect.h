#pragma once
#include "ParticleSystem.h"
class HarpoonEffect :
	public ParticleSystem
{
public:
	HarpoonEffect(int playerid);
	virtual ~HarpoonEffect(void);
	virtual void resetParticle(ParticleAttributes* a);
	virtual void update(float timeDelta);
	virtual void setPosition(Vec3f pos, int size);

	D3DXVECTOR3 pos;
	D3DXVECTOR3 prevPos;

	int playerid;
};

