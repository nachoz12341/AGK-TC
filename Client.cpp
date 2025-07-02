#include "Client.h"
#include "agk.h"
#include <map>

Client::Client(const char* address, int port): syncManager(SyncManager::Client), reliableListener("anyip4", port)
{
	serverAddress = address;
	serverPort = port;
	serverUUID = nullptr;

	fastListener = agk::CreateUDPListener("anyip4", port+1); // Create a UDP listener for fast messages
}

// Cleanup if necessary
Client::~Client() 
{
	//Try to clean disconnect
	if (IsConnected())
	{
		reliableListener.Disconnect(serverUUID);
	}

	agk::DeleteUDPListener(fastListener); // Delete the fast listener
}

void Client::Update()
{
	reliableListener.Update(); // Update the listener to process incoming messages

	//Get connection
	RUDPListener::ConnectionUUID connection = reliableListener.GetConnectionByAddress(serverAddress, serverPort); // Get the connection by address and port
	
	if (connection == nullptr)
	{
		//Continue trying to connnect
		reliableListener.Connect(serverAddress, serverPort);
		std::string s = "Connecting to server at " + std::string(serverAddress) + ":" + std::to_string(serverPort);
		agk::Print(s.c_str());

		serverUUID = nullptr; // Set connectionUUID to nullptr if not connected
	}
	else
	{
		serverUUID = connection;
		
		SyncManager::SyncDataMap updateMap; // Create a map to hold sync data updates
		syncManager.EncodeSyncData(updateMap); // Encode sync data for the connection
	}

	syncManager.SyncUpdate(); // Ticks all sync updates
}

void Client::RegisterObject(SyncObj* syncObj)
{
	syncManager.AddSync(syncObj->GetUUID(), syncObj, SyncManager::Client); // Register the sync object with the sync manager
}

bool Client::IsConnected() const
{
	return serverUUID != nullptr; // Check if the client is connected to the server
}
