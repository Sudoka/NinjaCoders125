#include "HarpoonEffect.h"


HarpoonEffect::HarpoonEffect(void)
{
	vbSize = 2046;
	vbOffset = 0;
	vbBatchSize = 512;
	pointSize = 2.5f;
}


HarpoonEffect::~HarpoonEffect(void)
{
	particles.clear();
}

void HarpoonEffect::resetParticle(ParticleAttributes* a)
{
	if(this->pos == this->prevPos && this->particles.size() > 1)
	{
		a->isAlive = false;
	}
	else {
		a->isAlive = true;
		a->pos = pos;
		a->age = 0;
		a->lifetime = 5;
		a->color = D3DXCOLOR(1.0f,0.0f,1.0f,1.0f);
	}
}

void HarpoonEffect::update(float timeDelta)
{
	list<ParticleAttributes>::iterator i;
	for(i = particles.begin(); i != particles.end(); i++)
	{
		i->age+=timeDelta;
		if(i->age > i->lifetime) resetParticle(&(*i));
		
	}
	this->removeDeadParticles();
}

void HarpoonEffect::setPosition(Vec3f pos, int size)
{
	this->prevPos = this->pos;
	this->pointSize = size;
	this->pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
	addParticle();
}