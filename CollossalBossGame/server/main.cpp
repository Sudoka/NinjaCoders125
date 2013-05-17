/*
 * main.cpp
 */

#include "GameServer.h"
#include "defs.h"

#include <windows.h>
#include <windowsx.h>

int main(int argc, char **argv) {

	GameServer::init();
	GameServer::get()->run();
	GameServer::clean();

	return 0;
}
