#include "ClientObjectManager.h"
#include "RenderEngine.h"

//Objects the COM can create
#include "TestObject.h"

ClientObjectManager *ClientObjectManager::com;

ClientObjectManager::ClientObjectManager(void) {
	curId = 0;
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
	CreateHeader *h;
	switch(cmd) {
	case CMD_CREATE:
		h = (CreateHeader*)data;
		create(id, h->type, data);
		break;
	case CMD_UPDATE:
		//In case a packet gets dropped, I'm lumping these two together.
		it = mObjs.find(id);
		if(it != mObjs.end()) {
			it->second->deserialize(data);
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
void ClientObjectManager::create(uint id, ObjectType type, char *data) {
	ClientObject *obj;
	//ClientObject *obj1;
	switch(type) {
	case OBJ_PLAYER:
	default:	//OBJ_GENERAL
		//TODO: Add more logic
		obj = new TestObject(id, data, "smallBox.x");
		//obj1 = new TestObject(id+1, data, "smallBox.x");
		//obj1->getRenderModel()->getFrameOfRef()->setPos(Point_t(0, 0, 10));
		break;
	}
	add(obj);
	//add(obj1);
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

