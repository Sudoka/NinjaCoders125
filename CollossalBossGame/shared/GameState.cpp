#include "GameState.h"
#include <windows.h>
#include "assert.h"

GameState::GameState(void)
{
	this->reset();
}

GameState::~GameState(void) { }

void GameState::reset() {
	SecureZeroMemory(this, sizeof(GameState));
}

int GameState::serialize(char * buf) {
	memcpy(buf, this, sizeof(GameState));
	return sizeof(GameState);
}

void GameState::deserialize(char * buf) {
	memcpy(this, buf, sizeof(GameState));
}

void GameState::playerready(int playerid) {
	playersallready = true;
	for(int i = 0; i < totalPlayerCount; i++) {
		if(playersconnected[i] == playerid+1) {
			playersready[i] = true;
		}
		if(playersready[i] == false) {
			playersallready = false;
		}
	}
}

void GameState::clientready(int playerid) {
	playersready[getplayerindex(playerid)] = true;

	clientsallready = true;
	for(int i = 0; i < totalPlayerCount; i++) {
		if(playersready[i] == false) {
			clientsallready = false;
		}
	}
}

int GameState::getplayerindex(int playerid) {
	for(int i = 0; i < 4; i++) {
		if(playersconnected[i] == playerid+1) {
			return i;
		}
	}
	return -1;
}

#pragma region World Events

void GameState::playerconnect(int playerid) {
	for(int i = 0; i < 4; i++) {
		if(playersconnected[i] == playerid+1) {
			return;
		}
	}
	playersconnected[totalPlayerCount] = playerid+1;
	totalPlayerCount++;
}

void GameState::playerdeath(int playerid) {
	playerDeathCount++;
	playerdeathstat[playerid]++;
}

void GameState::monsterdeath() {
	monsterDeathCount++;
	assert((monsterDeathCount <= totalMonsterCount) && "Implementation Error");
}

void GameState::monsterspawn() {
	totalMonsterCount++;
}

#pragma endregion END World Events