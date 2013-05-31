#include "CyborgCObj.h"
#include "RenderEngine.h"


CyborgCObj::CyborgCObj(uint id, char *data) : PlayerCObj(id, data)
{
	chargingEffect = new ChargeEffect(10);
	// Register with RE, SO SMART :O
	RE::get()->addParticleEffect(chargingEffect);
	
}


CyborgCObj::~CyborgCObj(void)
{
	RE::get()->removeParticleEffect(chargingEffect);
}

bool CyborgCObj::update() {
	chargingEffect->setPosition( rm->getFrameOfRef()->getPos(), (int)charge);
	chargingEffect->update(.33f);
	return PlayerCObj::update();
}

int CyborgCObj::getTypeInt()
{
	return 0;
}