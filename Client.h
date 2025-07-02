#ifndef _H_CLIENT
#define _H_CLIENT

#include <string>

#include "AGK-Reliable-UDP/RUDPListener.h"
#include "SyncManager.h"

class Client {

	public:
		Client(const char* address, int port) : serverAddress(address), serverPort(port), syncManager(SyncManager::Client), listener("anyip4", port) {};
		~Client();
		void Update();
		void RegisterObject(SyncObj* syncObj);
		bool IsConnected();

	private:
		const char* serverAddress;
		int serverPort;

		SyncManager syncManager;	//Sync manager for handling sync objects
		RUDPListener listener;		//Reliable UDP listener for network communication
};

#endif