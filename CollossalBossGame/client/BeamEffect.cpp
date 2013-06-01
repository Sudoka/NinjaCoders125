#include "BeamEffect.h"


BeamEffect::BeamEffect(void)
{
	vbSize = 2046;
	vbOffset = 0;
	vbBatchSize = 512;
	pointSize = 1.0f;
}


BeamEffect::~BeamEffect(void)
{
}

void BeamEffect::resetParticle(ParticleAttributes* a)
{
	static unsigned int i = 1;
	a->isAlive = true;
	a->color = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	// determine position
	D3DXVECTOR3 dist = (pos - origin) / (float)particles.size();
	a->pos = origin + dist * (float)i;
	i++;
	if(particles.size() != 0 && i % particles.size() == 0) i = 1;
}

void BeamEffect::update(float timeDelta)
{
	list<ParticleAttributes>::iterator i;
	for(i = particles.begin(); i != particles.end(); i++)
	{
		resetParticle(&(*i));
	}
}

void BeamEffect::setPosition(Vec3f pos, Vec3f origin, int size)
{
	this->origin = D3DXVECTOR3(origin.x, origin.y, origin.z);
	this->pointSize = (float)size;
	this->pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
	if(particles.size() < 30) addParticle();
}