#include "Client.h"
#include "agk.h"

// Cleanup if necessary
Client::~Client() 
{
	
}

void Client::Update()
{
	listener.Update(); // Update the listener to process incoming messages

	//Connected
	if (listener.GetConnectionByAddress(serverAddress, serverPort) == nullptr)
	{
		//Continue trying to connnect
		listener.Connect(serverAddress, serverPort);
		std::string s = "Connecting to server at " + std::string(serverAddress) + ":" + std::to_string(serverPort);
		agk::Print(s.c_str());
	}
	else
	{	
		
	}
	
	syncManager.Update(); // Update the sync manager to process sync objects
}

void Client::RegisterObject(SyncObj* syncObj)
{
	syncManager.AddSync(syncObj->GetUUID(), syncObj, SyncManager::Client); // Register the sync object with the sync manager
}

bool Client::IsConnected()
{
	return listener.GetConnectionByAddress(serverAddress, serverPort) != nullptr; // Check if the client is connected to the server
}
