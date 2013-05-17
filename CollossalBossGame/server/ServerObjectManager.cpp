#include "ServerNetworkManager.h"
#include "ServerObjectManager.h"
#include "ConfigurationManager.h"
#include "PhysicsEngine.h"
#include "PlayerSObj.h"

ServerObjectManager *ServerObjectManager::som;

ServerObjectManager::ServerObjectManager(void) {
	curId = 0;
	debugFlag = CM::get()->find_config_as_bool("SOM_DEBUG_FLAG");
	collisionMode = CM::get()->find_config_as_bool("COLLISION_MODE");
}


ServerObjectManager::~ServerObjectManager(void)
{
	for(map<uint, ServerObject *>::iterator it = mObjs.begin();
			it != mObjs.end();
			++it) {
		delete it->second;
	}
	mObjs.clear();
}

/*
 * Updates all objects
 *  logic
 *  physics
 *  collision checks
 *  deletes objects requesting deletion
 *  removes objects requesting removal
 *  adds objects requesting addtion
 *  tracks objects removed but not deleted
 *  adds changed objects to the server with appropriate commands
 */

void ServerObjectManager::update() {
	list<uint> lsObjsToDelete;
	list<uint>::iterator idIter;
	list<ServerObject *> lsObjsThatMoved;
	list<ServerObject *>::iterator objIter;
	for(map<uint, ServerObject *>::iterator it = mObjs.begin();
			it != mObjs.end();
			++it) {
		//Update object (game logic)
		if(it->second->update()) {
			//Object requested that it be deleted
			lsObjsToDelete.push_back(it->second->getId());
			continue;
		}
		
		//Update physics
		if(PE::get()->applyPhysics(it->second)) {
			//Add this object to the list of objects that have moved
			lsObjsThatMoved.push_back(it->second);
		} else {
			//Perform initial layer of collision checks against objects
			// that have moved, if this object has not moved
			for(objIter = lsObjsThatMoved.begin(); objIter != lsObjsThatMoved.end(); ++objIter) {
				PE::get()->applyPhysics(*objIter, it->second);
			}
		}
	}

	//Remove objects queued for deleting BEFORE collision checks occur
	for(idIter = lsObjsToDelete.begin(); idIter != lsObjsToDelete.end(); ++idIter) {
		map<uint, ServerObject *>::iterator itObj = mObjs.find(*idIter);
		if(itObj != mObjs.end()) {
			lsObjsToSend.push_back(pair<CommandTypes,ServerObject*>(CMD_DELETE,itObj->second));
			//ServerObject *obj = itObj->second;
			mObjs.erase(itObj);
			//delete obj;	We don't perform this until the object is sent
		}
	}
	lsObjsToDelete.clear();

	//Perform final layer of collision checks
	for(objIter = lsObjsThatMoved.begin(); objIter != lsObjsThatMoved.end(); ++objIter) {
		for(map<uint, ServerObject *>::iterator it = mObjs.begin();
				it != mObjs.end();
				++it) {
			//When we reach the same object, we are done
			if(it->first == (*objIter)->getId()) {
				//Send this object to the server
				lsObjsToSend.push_back(pair<CommandTypes,ServerObject*>(CMD_UPDATE,*objIter));
				break;
			} else {
				PE::get()->applyPhysics(*objIter, it->second);
			}
		}
	}

	//Remove objects requested for removal, but not deletion
	for(idIter = lsObjsRemoveQueue.begin(); idIter != lsObjsRemoveQueue.end(); ++idIter) {
		mObjs.erase(*idIter);
	}
	lsObjsRemoveQueue.clear();

	//Add objects requested for addition
	for(objIter = lsObjsAddQueue.begin(); objIter != lsObjsAddQueue.end(); ++objIter) {
		mObjs.insert(pair<uint,ServerObject*>((*objIter)->getId(),*objIter));
		lsObjsToSend.push_back(pair<CommandTypes,ServerObject*>(CMD_CREATE,*objIter));
	}
	lsObjsAddQueue.clear();
}

/*
 * Populates the vector with all client objects of ObjectType type.
 * Author: Franklin
 */
void ServerObjectManager::findObjects(ObjectType type, vector<ServerObject *> * l) {
	for(map<uint, ServerObject *>::iterator it = mObjs.begin(); it != mObjs.end(); ++it) {
		if (it->second->getType() == type) {
			l->push_back(it->second);
		}
	}
}

/**
 * Sends the object states to the clients.
 * Author: Haro
 * Modified by: Nathan
 */
void ServerObjectManager::sendState()
{
	char *buf;
	int datalen;
	int totalData = 0;
	for(list<pair<CommandTypes,ServerObject*> >::iterator it = lsObjsToSend.begin();
			it != lsObjsToSend.end(); ++it) {
		//Initialize the buffer with the object header
		buf = SNM::get()->getSendBuffer();
		switch(it->first) {
		case CMD_UPDATE:
		case CMD_CREATE: {
			//Fill out the header
			CreateHeader *h = (CreateHeader*)buf;
			h->type = it->second->getType();
			if(it->second->getType() == OBJ_PLAYER) {
				PlayerSObj * pso = (PlayerSObj *)(it->second);
				h->cc = pso->charclass;
			}
			//Serialize the object
			datalen = it->second->serialize(buf + sizeof(CreateHeader)) + sizeof(CreateHeader);
			totalData += datalen;
			SNM::get()->sendToAll(OBJECT_MANAGER, it->second->getId(), it->first, datalen);
			break;
			}
		case CMD_DELETE:
			datalen = 0;
			SNM::get()->sendToAll(OBJECT_MANAGER, it->second->getId(), it->first, datalen);
			delete it->second;	//We are finally done with this object
		}
	}
	lsObjsToSend.clear();
	//DC::get()->print("Total data sent to client is %d\n", totalData);
	SNM::get()->getSendBuffer();
	SNM::get()->sendToAll(COMPLETE, 0);
/*
	for(map<uint, ServerObject *>::iterator it = mObjs.begin();
			it != mObjs.end();
			++it) {
		// If object changed...
		int datalen = it->second->serialize(ServerNetworkManager::get()->getSendBuffer());
		ServerNetworkManager::get()->sendToAll(OBJECT_MANAGER, it->second->getId(), datalen);
	}
*/
}

uint ServerObjectManager::genId() {
	if(vFreeIds.size() == 0) {
		return curId++;
	}

	//Recycle an existing id
	uint id = vFreeIds[vFreeIds.size() - 1];
	vFreeIds.pop_back();
	return id;
}

void ServerObjectManager::freeId(uint id) {
	//Mark this ID as being ready for recycling
	vFreeIds.push_back(id);
}

void ServerObjectManager::add(ServerObject *obj) {
	//mObjs.insert(pair<uint,ServerObject*>(obj->getId(), obj));
	lsObjsAddQueue.push_back(obj);
}

ServerObject *ServerObjectManager::find(uint id) {
	map<uint, ServerObject*>::iterator res = mObjs.find(id);
	if(res != mObjs.end()) {
		return res->second;
	}
	return NULL;
}

ServerObject *ServerObjectManager::remove(uint id) {
	lsObjsRemoveQueue.push_back(id);
	return find(id);
}

void ServerObjectManager::reset() {
	// list<uint> asdf;
	list<ServerObject*> lsPlayers;
	list<uint> lsObjsToDelete; // Haro

	for(map<uint, ServerObject *>::iterator it = mObjs.begin();
			it != mObjs.end();
			++it) {
		ServerObject * o = it->second;
		// if it's not a Player object...
		if(o->getType() != OBJ_PLAYER) {
			// asdf.push_back(it->first);
			//freeId(it->first);
			//lsObjsToSend.push_back(pair<CommandTypes,ServerObject*>(CMD_DELETE,it->second));
			lsObjsToDelete.push_back(it->second->getId()); // Haro
			// delete o;
		}
		// if it is...
		else {
			o->initialize();
			lsPlayers.push_back(o);
		}
	}


	// Haro added this
	for(list<uint>::iterator idIter = lsObjsToDelete.begin(); idIter != lsObjsToDelete.end(); ++idIter) {
		map<uint, ServerObject *>::iterator itObj = mObjs.find(*idIter);
		if(itObj != mObjs.end()) {
			lsObjsToSend.push_back(pair<CommandTypes,ServerObject*>(CMD_DELETE,itObj->second));
			//ServerObject *obj = itObj->second;
			mObjs.erase(itObj);
			//delete obj;	We don't perform this until the object is sent
		}
	}
	lsObjsToDelete.clear();

	// Haro commented this out
	//mObjs.clear();


	/*
	for(list<uint>::iterator it = asdf.begin();
			it != asdf.end();
			++it) {
			mObjs.erase(*it);
	}
	*/
	

	// Haro commented this out
	/*for(list<ServerObject*>::iterator it = lsPlayers.begin();
			it != lsPlayers.end();
			++it) {
		add(*it);
	}
	lsPlayers.clear();*/
	
}