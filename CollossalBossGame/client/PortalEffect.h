#pragma once
#include "particlesystem.h"
class PortalEffect :
	public ParticleSystem
{
public:
	PortalEffect(void);
	virtual ~PortalEffect(void);
	virtual void resetParticle(ParticleAttributes* a);
	virtual void update(float timeDelta);
	virtual void setPosition(Vec3f pos);
	D3DXVECTOR3 pos;
	int res;
	int moved;
};

