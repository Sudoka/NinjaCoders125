#include "ClientObjectManager.h"
#include "RenderEngine.h"
#include "ConfigurationManager.h"

//Objects the COM can create
#include "TestObject.h"
#include "PlayerCObj.h"
#include "MonsterCObj.h"

ClientObjectManager *ClientObjectManager::com;

ClientObjectManager::ClientObjectManager(void) {
	curId = 0;
	debugFlag = CM::get()->find_config_as_bool("COM_DEBUG_FLAG");
}


ClientObjectManager::~ClientObjectManager(void) {
	for(map<uint, ClientObject *>::iterator it = mObjs.begin();
			it != mObjs.end();
			++it) {
		delete it->second;
	}
	mObjs.clear();
}

/*
 * Performs logic updates
 */
#include <list>

void ClientObjectManager::update() {
	list<uint> lsObjsToDelete;
	for(map<uint, ClientObject *>::iterator it = mObjs.begin();
			it != mObjs.end();
			++it) {
		//Update object (game logic)
		if(it->second->update()) {
			//Object requested that it be deleted
			lsObjsToDelete.push_back(it->second->getId());
			continue;
		}

		//Give objects to the render engine for rendering
		RE::get()->renderThis(it->second);
	}

	//Remove objects queued for deleting BEFORE collision checks occur
	for(list<uint>::iterator itDel = lsObjsToDelete.begin();
			itDel != lsObjsToDelete.end();
			++itDel) {
		map<uint, ClientObject *>::iterator itObj = mObjs.find(*itDel);
		ClientObject *obj = itObj->second;
		mObjs.erase(itObj);
		delete obj;
	}
	lsObjsToDelete.clear();
}

ClientObject *ClientObjectManager::find(uint id) {
	map<uint, ClientObject*>::iterator res = mObjs.find(id);
	if(res != mObjs.end()) {
		return res->second;
	}
	return NULL;
}

void ClientObjectManager::serverUpdate(uint id, CommandTypes cmd, char *data) {
	map<uint,ClientObject*>::iterator it;
	switch(cmd) {
	case CMD_UPDATE:
	case CMD_CREATE:
		//The client may connect AFTER all the objects have been created.
		it = mObjs.find(id);
		if(it != mObjs.end()) {
			it->second->deserialize(data + sizeof(CreateHeader));
		} else {
			create(id, data);
		}
		break;
	case CMD_DELETE:
		it = mObjs.find(id);
		if(it != mObjs.end()) {
			ClientObject *cobj = it->second;
			mObjs.erase(it);
			delete cobj;
		}
		break;
	}
}

/*
 * Creates the specified object and adds it to the list
 */
void ClientObjectManager::create(uint id, char *data) {
	ClientObject *obj;
	CreateHeader *h = (CreateHeader*)data;
	switch(h->type) {
	case OBJ_PLAYER:
		obj = new PlayerCObj(id, data + sizeof(CreateHeader));
		break;
	case OBJ_MONSTER:
		obj = new MonsterCObj(id, data + sizeof(CreateHeader));
		break;
	//case OBJ_ARENA:
	//	obj = new WallCObj(id, data + sizeof(CreateHeader));
	default:	//OBJ_GENERAL
		obj = new TestObject(id, data + sizeof(CreateHeader));
		break;
	}
	add(obj);
}

void ClientObjectManager::add(ClientObject *obj) {
	mObjs.insert(pair<uint,ClientObject*>(obj->getId(), obj));
}

#if 0
uint ClientObjectManager::genId() {
	if(vFreeIds.size() == 0) {
		return curId++;
	}

	//Recycle an existing id
	uint id = vFreeIds[vFreeIds.size() - 1];
	vFreeIds.pop_back();
	return id;
}

void ClientObjectManager::freeId(uint id) {
	//Mark this ID as being ready for recycling
	vFreeIds.push_back(id);
}


void ClientObjectManager::remove(uint id) {
	mObjs.erase(id);
}
#endif

