#include "ServerObject.h"
#include "ServerObjectManager.h"

ServerObject::ServerObject(uint id) {
	this->id = id;
	flags = 0;

	cm = new CollisionModel();

	//Set default flags
	setFlag(IS_FALLING, true);
	setFlag(IS_FLOATING, false);
}


ServerObject::~ServerObject(void) {
	SOM::get()->freeId(id);
	delete cm;
}
