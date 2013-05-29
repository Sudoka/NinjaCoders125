#include "HarpoonEffect.h"


HarpoonEffect::HarpoonEffect()
{
	vbSize = 2046;
	vbOffset = 0;
	vbBatchSize = 512;
	pointSize = 2.5f;
	//this->playerid = playerid;
	//for(int i = 0; i < 10; i++) addParticle();
}


HarpoonEffect::~HarpoonEffect(void)
{
	particles.clear();
}

void HarpoonEffect::resetParticle(ParticleAttributes* a)
{
	/*if(this->pos == this->prevPos && this->particles.size() > 1)
	{
		a->isAlive = false;
	}
	else {
		a->isAlive = true;
		a->pos = pos;
		a->age = 0.9f;
		a->lifetime = 1;
		a->color = D3DXCOLOR(1.0f,0.0f,1.0f,1.0f);
	}*/
	static int i = 1;
	a->isAlive = true;
	//a->age = 0.f;
	//a->lifetime = 1;
	a->color = D3DXCOLOR(1.0f,0.0f,1.0f,1.0f);
	// determine position
	D3DXVECTOR3 dist = (pos - origin) / particles.size();
	a->pos = origin + dist * i;
	i++;
	if(particles.size() != 0 && i % particles.size() == 0) i = 1;
}

void HarpoonEffect::update(float timeDelta)
{
	//particles.clear();
	
	list<ParticleAttributes>::iterator i;
	for(i = particles.begin(); i != particles.end(); i++)
	{
		//i->age+=timeDelta;
		//if(i->age > i->lifetime) resetParticle(&(*i));
		resetParticle(&(*i));
	}
	//this->removeDeadParticles();
	
}

void HarpoonEffect::setPosition(Vec3f pos, Vec3f origin, int size)
{
//	this->prevPos = this->pos;
	this->origin = D3DXVECTOR3(origin.x, origin.y, origin.z);
	this->pointSize = size;
	this->pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
	if(particles.size() < 10) addParticle();
}