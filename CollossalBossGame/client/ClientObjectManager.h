#pragma once

#include <vector>
#include <map>

using namespace std;

#include "defs.h"
#include "ClientObject.h"
#include "NetworkData.h"

class ClientObjectManager
{
public:
	static void init() { com = new ClientObjectManager(); }
	static ClientObjectManager *get() { return com; }
	static void clean() { delete com; }

	void update();
	void findObjects(ObjectType type, vector<ClientObject *> * l);

	WorldState *getWorldState() { return &worldState; }

	//Object handling
	ClientObject *find(uint id);
	void serverUpdate(uint id, CommandTypes cmd, char *data);

	int player_id;
	bool debugFlag;	
	bool collisionMode;
/*  Needs to receive a server update
	uint genId();
	void freeId(uint id);
	void remove(uint id);
*/

private:
	ClientObjectManager(void);
	virtual ~ClientObjectManager(void);
	void create(uint id, char *data);
	void add(ClientObject *obj);

	static ClientObjectManager *com;

	map<uint, ClientObject *> mObjs;
	vector<uint> vFreeIds;
	int curId;
	WorldState worldState;
};
typedef ClientObjectManager COM;
