#ifndef _H_SERVER
#define _H_SERVER

#include <string>
#include "AGK-Reliable-UDP/RUDPListener.h"
#include "SyncManager.h"

class Server {
	public:
		Server();
		~Server();
		void Update();
	private:
		unsigned int fastListener;
		RUDPListener reliableListener;
		SyncManager syncManager;	

		void SendRPCData();
		void SendFastData();

		void ReceiveRPCData();
		void ReceiveFastData();
};

#endif
