#include "GameServer.h"
#include "ServerNetworkManager.h"
#include "ConfigurationManager.h"
#include "ServerObjectManager.h"
// #include "NetworkData.h"
#include "PhysicsEngine.h"
#include "PlayerSObj.h"
#include "game.h"
#include "Action.h"
#include <Windows.h>
#include <assert.h>
#include <iostream>

using namespace std;

#define TICK 1000/30 // 30 times per second (in ms)

GameServer * GameServer::gs;

GameServer::GameServer(void)
{
	initializeSubModules();
	state.reset();
}

GameServer::~GameServer(void)
{
	cleanSubModules();
	state.reset();
}

void GameServer::initializeSubModules() {
	DC::init("serverLog.txt");
	SNM::init();
	SOM::init();
	PE::init();
}

void GameServer::cleanSubModules() {
	SOM::clean();
	PE::clean();
	SNM::clean();
	DC::clean();
}

void GameServer::gameLoop() {
	// Get input from client
    SNM::get()->update();

	// Update state
	SOM::get()->update();

	// Send state to client
	SOM::get()->sendState();

	// Send current game state
	this->sendGameState();

	// Complete Send
	SNM::get()->getSendBuffer();
	SNM::get()->sendToAll(COMPLETE, 0);
}

/* the main function on the server side, initalizes the server loop
*/
void GameServer::run() {
	//Create game objects
	gameInit();

	//Main server loop
	while(true) 
    {
		// Get timestamp
		time(&beginLoopTimer);

		gameLoop();

		// Wait until next clock tick
		time(&endLoopTimer);
		totalLoopTime = difftime(endLoopTimer, beginLoopTimer)  * 1000; // in ms
		// Be sure to set debug to the right thing!
		if (totalLoopTime < TICK) {
			Sleep((DWORD)(TICK - totalLoopTime));
		}
		else
		{
			int tick = TICK;
			DC::get()->print("WARNING!!! total loop time %f is greater than tick time: %d\n...NOTE: this might mean a client is connecting\n", totalLoopTime, tick);
		}
    }
}

void GameServer::sendGameState() {
	char * buf = SNM::get()->getSendBuffer();
	int dlen = state.serialize(buf);
	SNM::get()->sendToAll(GAMESTATE_MANAGER, dlen);
}

void GameServer::recieveInput(char * buf, int pid) {
	inputstatus istat;
	memcpy(&istat, buf, sizeof(inputstatus));
	if(istat.start && istat.quit) {
		// Do nothing
	} else if(istat.quit) {
		state.gameover = true;
	} else if(istat.start) {
		if(state.gameover) {
			vector<ServerObject *> rawr;
			SOM::get()->findObjects(OBJ_PLAYER, &rawr);
			for(unsigned int i = 0; i < rawr.size(); i++) {
				PlayerSObj * p = reinterpret_cast<PlayerSObj *>(rawr[i]);
				if(p) { p->ready = false; }
			}
		}
		state.playerready(pid);
	}
}

void GameServer::event_reset(int playerid) { event_hard_reset(playerid); }
void GameServer::event_hard_reset(int playerid) {	
	if(state.playerDeathCount == state.totalPlayerCount || state.monsterDeathCount == state.totalMonsterCount) {
		state.totalMonsterCount = 0;
		state.playerDeathCount = 0;
		state.monsterDeathCount = 0;
		// Fire some function to reset the positions of all server objects.
		SOM::get()->reset();
		gameInit();
	}
}

void GameServer::event_player_death(int playerid) { state.playerdeath(playerid); }
void GameServer::event_monster_death() { state.monsterdeath(); }
void GameServer::event_connection(int playerid) { state.playerconnect(playerid); }
void GameServer::event_monster_spawn() { state.monsterspawn(); }
void GameServer::event_disconnect(int playerid) { state.totalPlayerCount--; }