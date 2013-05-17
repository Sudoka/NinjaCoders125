#pragma once
#include "ParticleSystem.h"
class ChargeEffect :
	public ParticleSystem
{
public:
	ChargeEffect(int numParticles);
	virtual ~ChargeEffect(void);
	virtual void resetParticle(ParticleAttributes* a);
	virtual void update(float timeDelta);
	virtual void setPosition(Vec3f pos, int charge);

	int numParticles;
	int charge, killCounter;
	D3DXVECTOR3 min;
	D3DXVECTOR3 max;
	D3DXVECTOR3 middle;
};

