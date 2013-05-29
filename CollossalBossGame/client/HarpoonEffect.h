#pragma once
#include "ParticleSystem.h"
class HarpoonEffect :
	public ParticleSystem
{
public:
	HarpoonEffect();
	virtual ~HarpoonEffect(void);
	virtual void resetParticle(ParticleAttributes* a);
	virtual void update(float timeDelta);
	virtual void setPosition(Vec3f pos, Vec3f origin, int size);

	D3DXVECTOR3 pos;
	D3DXVECTOR3 prevPos;
	D3DXVECTOR3 origin;

	//int playerid;
};

