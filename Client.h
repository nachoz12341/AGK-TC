#ifndef _H_CLIENT
#define _H_CLIENT

#include <string>

#include "AGK-Reliable-UDP/RUDPListener.h"
#include "SyncManager.h"

class Client {

	public:
		Client(const char* address, int port);
		~Client();
		void Update();
		void RegisterObject(SyncObj* syncObj);
		bool IsConnected() const;

	private:
		const char* serverAddress;
		int serverPort;

		SyncManager syncManager;	//Sync manager for handling sync objects
		RUDPListener reliableListener;		//Reliable UDP listener for network communication
		RUDPListener::ConnectionUUID serverUUID; // UUID of the connection to the server
		unsigned int fastListener;

		void SendRPCData();
		void SendFastData();
};

#endif