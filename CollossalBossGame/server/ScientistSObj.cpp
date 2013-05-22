#include "ScientistSObj.h"
#include "BulletSObj.h"
#include "PlayerSObj.h"
#include "ConfigurationManager.h"
#include "PhysicsEngine.h"
#include "ServerObjectManager.h"

ScientistSObj::ScientistSObj(uint id, uint clientId) : PlayerSObj(id, clientId, CHAR_CLASS_SCIENTIST)
{
	// Other re-initializations (things that don't depend on parameters, like config)
	this->initialize();
}


ScientistSObj::~ScientistSObj(void)
{
}

void ScientistSObj::initialize() {
	hso = NULL;
}

void ScientistSObj::actionAttack() {

}

void ScientistSObj::actionCharge(bool buttondown) {

}

void ScientistSObj::clearAccessory() {
	
}