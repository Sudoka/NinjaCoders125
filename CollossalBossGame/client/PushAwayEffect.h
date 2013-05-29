#pragma once
#include "ParticleSystem.h"
class PushAwayEffect :
	public ParticleSystem
{
public:
	PushAwayEffect(void);
	virtual ~PushAwayEffect(void);
	virtual void resetParticle(ParticleAttributes* a);
	virtual void update(float timeDelta);
	virtual void setPosition(Vec3f pos);

	D3DXVECTOR3 pos;
};

