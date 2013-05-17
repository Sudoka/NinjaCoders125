#pragma once

#include <vector>
#include <map>
#include <list>

using namespace std;

#include "defs.h"
#include "ServerObject.h"
#include "NetworkData.h"

class ServerObjectManager
{
public:
	static void init() { som = new ServerObjectManager(); }
	static ServerObjectManager *get() { return som; }
	static void clean() { delete som; }

	void reset();
	void update();
	void sendState();

	//Object handling
	uint genId();
	void freeId(uint id);
	void add(ServerObject *obj);
	ServerObject *find(uint id);
	ServerObject *remove(uint id);
	void findObjects(ObjectType type, vector<ServerObject *> * l);

	bool debugFlag;
	bool collisionMode;

private:
	ServerObjectManager(void);
	virtual ~ServerObjectManager(void);

	static ServerObjectManager *som;

	map<uint, ServerObject *> mObjs;
	vector<uint> vFreeIds;
	list<ServerObject *> lsObjsAddQueue;
	list<uint> lsObjsRemoveQueue;
	list<pair<CommandTypes,ServerObject*> > lsObjsToSend;
	int curId;
};
typedef ServerObjectManager SOM;
