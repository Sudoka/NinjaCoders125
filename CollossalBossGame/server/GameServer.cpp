#include "GameServer.h"
#include "ConfigurationManager.h"
#include "ServerObjectManager.h"
#include <Windows.h>
#include <assert.h>
#include <iostream>
#include <time.h> 
using namespace std;

#define TICK 1000/30 // 30 times per second (in ms)

GameServer::GameServer(void)
{

}

/* the main function on the server side, initalizes the server loop

*/
int main()
{
	// CM::get();
	// cout << CM::get()->find_config("asdf") << endl;
	// system("pause");
	SOM::init();
	DC::init("serverLog.txt");

	// Keep track of how long our updates take
	time_t beginLoopTimer;
	time_t endLoopTimer;
	double totalLoopTime;

	while(true) 
    {
		// Get timestamp
		time(&beginLoopTimer);

		// Get input from client
        ServerNetworkManager::get()->update();

		// Update state
		SOM::get()->update();

		// Send state to client
		SOM::get()->sendState();

		// Wait until next clock tick
		time(&endLoopTimer);
		totalLoopTime = difftime(endLoopTimer, beginLoopTimer)  * 1000; // in ms

		// Be sure to set debug to the right thing!
		if (totalLoopTime < TICK) {
			Sleep(TICK - totalLoopTime);
		}
		else
		{
			// TODO: Print to error console
			DC::get()->print("ERROR!!! total loop time %d is greater than tick time: %d\n", totalLoopTime, TICK);
		}
		
    }

	SOM::clean();
	DC::clean();
}
