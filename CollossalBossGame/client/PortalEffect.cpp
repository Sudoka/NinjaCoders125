#include "PortalEffect.h"


PortalEffect::PortalEffect(void)
{
	vbSize = 2048;
	filename = "res/spiky.jpg";
	vbOffset = 0;
	vbBatchSize = 512;
	pointSize = 2.0f;
	for(int i = 0; i < 360; i++) addParticle();
	res = 360;
	moved = 0;
}


PortalEffect::~PortalEffect(void)
{
}

void PortalEffect::resetParticle(ParticleAttributes* a)
{
	a->isAlive = true;
	a->pos = pos;

	a->color = D3DXCOLOR(0.0f,1.0f,0.0f,1.0f);
}

void PortalEffect::update(float timeDelta)
{
	int radius = 40;
	list<ParticleAttributes>::iterator i;
	for(i = particles.begin(); i != particles.end(); i++)
	{
		resetParticle(&(*i));
		i->pos.x += (float)(cos(2 * M_PI * moved / res)) * radius;
		i->pos.y += (float)(sin(2 * M_PI * moved / res)) * radius;
		moved = moved++ % res;
	}
}

void PortalEffect::setPosition(Vec3f pos)
{
	this->pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
}

