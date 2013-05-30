#pragma once
#include "ParticleSystem.h"
class Snow :
	public ParticleSystem
{
public:
	Snow(int numParticles);
	virtual ~Snow(void);
	virtual void resetParticle(ParticleAttributes* a);
	virtual void update(float timeDelta);


};

