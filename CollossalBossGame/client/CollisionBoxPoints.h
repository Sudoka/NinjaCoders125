#pragma once
#include "ParticleSystem.h"
#include <vector>
class CollisionBoxPoints :
	public ParticleSystem
{
public:
	CollisionBoxPoints(void);
	virtual ~CollisionBoxPoints(void);
	virtual void resetParticle(ParticleAttributes* a);
	virtual void update(float timeDelta);
	virtual void addParticles(vector<Vec3f> pts);

};

