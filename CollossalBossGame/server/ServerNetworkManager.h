#pragma once

#include <winsock2.h>
#include <Windows.h>
#include "NetworkServices.h"
#include "NetworkData.h"
#include <ws2tcpip.h>
#include <map>
using namespace std; 
#pragma comment (lib, "Ws2_32.lib")

#define CUSTOM_CLIENT_CRASH_ERROR_CODE -256

class ServerNetworkManager
{
public:
	static void init() { SNM = new ServerNetworkManager(); }
	static ServerNetworkManager * get() { return SNM; }
	static void clean() { delete SNM; }

	static unsigned int client_id;		// Unique Client Ids for each connecting client TODO private, numClients

	void update();												// Generic Update cycle - manages buffer and connections.
	void receiveFromClients();									// Collects data from all clients
	bool acceptNewClient(unsigned int & id);					// accept new connections
    int  receiveData(unsigned int client_id, char * recvbuf);	// receive incoming data
	char* getSendBuffer();										// Fetches send buffer to fill for sending.

	// send data to a single client
	int sendToClient(SOCKET sock_id, unsigned int packet_type, unsigned int data_size);
	int sendToClient(SOCKET sock_id, unsigned int packet_type, unsigned int object_id, unsigned int data_size);
	int sendToClient(SOCKET sock_id, unsigned int packet_type, unsigned int object_id, CommandTypes command_type, unsigned int data_size);
	int sendToClient(SOCKET sock_id, unsigned int iteration, unsigned int packet_type, unsigned int object_id, CommandTypes command_type, unsigned int data_size);
    // send data to all clients
	void sendToAll(unsigned int packet_type, unsigned int data_size);
	void sendToAll(unsigned int packet_type, unsigned int object_id, unsigned int data_size);
	void sendToAll(unsigned int packet_type, unsigned int object_id, CommandTypes command_type, unsigned int data_size);
	void sendToAll(unsigned int iteration, unsigned int packet_type, unsigned int object_id, CommandTypes command_type, unsigned int data_size);

    SOCKET ListenSocket;	// Socket to listen for new connections
    SOCKET ClientSocket;	// Socket to give to the clients
    int iResult;			// for error checking return values
	unsigned int iteration;	// The iteration count of the server
	unsigned int p_count;	// The number assigned to each individual packet
    std::map<unsigned int, SOCKET> sessions; // table to keep track of each client's socket
	std::map<long, unsigned int> sessionsip; // table to keep track of each client's ipaddress in the form of a long
	std::map<unsigned int, unsigned int> sessionsobjid; // table <client id, object id>
	Packet send_buffer;

private:
	ServerNetworkManager(void);
    ~ServerNetworkManager(void);

	SOCKET getSocketById(int client_id);
	inline int initPacketBuffer(unsigned int iteration, unsigned int packet_type, unsigned int object_id, CommandTypes command_type, unsigned int data_size);

	static ServerNetworkManager * SNM;	// Class Singleton
	char network_data[MAX_PACKET_SIZE];	// data buffer
	char packet_buffer[MAX_PACKET_SIZE];	// data buffer
	bool prepare_packet;

	bool debugFlag;
};
typedef ServerNetworkManager SNM;
