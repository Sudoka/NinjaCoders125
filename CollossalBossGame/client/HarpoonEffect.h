#pragma once
#include "ParticleSystem.h"
class HarpoonEffect :
	public ParticleSystem
{
public:
	HarpoonEffect(void);
	virtual ~HarpoonEffect(void);
	virtual void resetParticle(ParticleAttributes* a);
	virtual void update(float timeDelta);
	virtual void setPosition(Vec3f pos);
};

