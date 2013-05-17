#pragma once
#include "ParticleSystem.h"
class ShootingEffect :
	public ParticleSystem
{
public:
	ShootingEffect(void);
	virtual ~ShootingEffect(void);
	virtual void resetParticle(ParticleAttributes* a);
	virtual void update(float timeDelta);
	virtual void setPosition(Vec3f pos);

	D3DXVECTOR3 pos;
};

