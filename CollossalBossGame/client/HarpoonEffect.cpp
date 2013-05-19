#include "HarpoonEffect.h"


HarpoonEffect::HarpoonEffect(void)
{
	vbSize = 2046;
	vbOffset = 0;
	vbBatchSize = 512;
	pointSize = 15.5f;
	addParticle();
}


HarpoonEffect::~HarpoonEffect(void)
{
	particles.clear();
}
