#include "ClientObject.h"
#include "ClientObjectManager.h"

ClientObject::ClientObject(uint id) {
	this->id = id;
	this->type = OBJ_GENERAL;
}

ClientObject::ClientObject(uint id, ObjectType type) {
	this->id = id;
	this->type = type;
}

ClientObject::~ClientObject(void) {
}
