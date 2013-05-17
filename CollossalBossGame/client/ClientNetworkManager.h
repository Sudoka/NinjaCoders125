// Networking libraries
#ifndef CLIENT_NETWORK_MANAGER_H
#define CLIENT_NETWORK_MANAGER_H

#include "NetworkServices.h"
#include "NetworkData.h"
#include <ws2tcpip.h>
#include <stdio.h>
#include <vector>

// size of our buffer
#define DEFAULT_BUFLEN 512
// port to connect sockets through 
#define DEFAULT_PORT "6881"
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

//each client has a client networker
class ClientNetworkManager {
public:
	static void init() { CNM = new ClientNetworkManager(); }
	static ClientNetworkManager * get() { return CNM; }
	static void clean() { delete CNM; }

    int iResult;			// for error checking function calls in Winsock library
    SOCKET ConnectSocket;	// socket for client to connect to server
	char network_data[MAX_PACKET_SIZE];

	int receivePackets(char *);
	bool update();
	void sendData(PacketTypes messagetype, char * data, int datalen, int objectID);
	bool isConnected();
private:
	// ctor/dtor
    ClientNetworkManager(void);
    ~ClientNetworkManager(void);

	static ClientNetworkManager * CNM;
	static unsigned int exper;
	unsigned int iteration_count;
	unsigned int response_packet_number;
	bool connected;
	int debugFlag;
};
typedef ClientNetworkManager CNM;
#endif