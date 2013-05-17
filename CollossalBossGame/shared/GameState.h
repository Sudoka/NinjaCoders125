#pragma once

#include <vector>
#include <string>

using namespace std;

class GameState
{
public:
	GameState(void);
	~GameState(void);

	void reset();
	int serialize(char * buf);
	void deserialize(char * buf);

	int getplayerindex(int playerid);

	void playerconnect(int playerid);
	void playerdeath(int playerid);
	void monsterspawn();
	void monsterdeath();

	void playerready(int playerid);
	void clientready(int playerid);

	void classselect(int playerid, bool inc, bool dec);

	// World States
	int currentState;

	// World Statistics
	int totalMonsterCount;
	int totalPlayerCount;
	int playerDeathCount;
	int monsterDeathCount;

	int playersconnected[4];
	bool clientsready[4];
	bool playersready[4];

	int classselection[4];
	int sceneselection;
};

enum GameStates {
	GAME_CONNECTING = 0,
	GAME_SCENE_SELECT, //1
	GAME_LOADING, //2
	GAME_CLASS_SELECT, //3
	GAME_START, //4
	GAME_RUNNING, //5
	GAME_END //6
};