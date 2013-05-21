#include "GameServer.h"
#include "ServerNetworkManager.h"
#include "ConfigurationManager.h"
#include "ServerObjectManager.h"
// #include "NetworkData.h"
#include "PhysicsEngine.h"
#include "game.h"
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
	SNM::get()->sendToAll(GAMESTATE_MANAGER, -1, dlen);
}

void GameServer::recieveInput(char * buf) {
	GameData gd;
	memcpy(&gd, buf, sizeof(GameData));

	if(gd.hardreset) {
		event_hard_reset(gd.playerid);
	}
	switch(state.currentState) {
		case GAME_CONNECTING:
			break;
		case GAME_SCENE_SELECT:
			if(gd.left && (state.getplayerindex(gd.playerid) == 0)) { state.sceneselection--; }
			if(gd.right && (state.getplayerindex(gd.playerid) == 0)) { state.sceneselection++; }
 			if(gd.start) { 
				state.currentState = GAME_LOADING; 
				// Initialize the world depending on the current scene selection.
				gameInit();
			}
			break;
		case GAME_LOADING:
			if(gd.clientready) { state.clientready(gd.playerid); }
			break;
		case GAME_CLASS_SELECT:
			if(gd.left || gd.right) {
				state.classselect(gd.playerid, gd.right, gd.left);
			}
			if(gd.right) { state.sceneselection++; }
			break;
		case GAME_START:
			if(gd.start) { state.playerready(gd.playerid); }
			break;
		case GAME_RUNNING:
			break;
		case GAME_END:
			if(gd.start) { event_reset(gd.playerid); }
			break;
		default:
			cout << "Odd State" << endl;
	}

	if(state.currentState == GAME_SCENE_SELECT || state.currentState == GAME_CLASS_SELECT) {

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