#include "Server.h"

#include "agk.h"

Server::Server(const char* address, int port) : reliableListener("anyip4", port), syncManager(SyncManager::Server)
{
	serverAddress = address; 
	serverPort = port; 
	fastListener = agk::CreateUDPListener("anyip4", port + 1); // Create a UDP listener for fast messages
}

Server::~Server()
{
	agk::DeleteUDPListener(fastListener); // Delete the fast listener
}

void Server::Update()
{
	reliableListener.Update(); // Update the listener to process incoming messages
	
	//Send Data
	SendRPCData();
	SendFastData();

	//Receive data
	ReceiveRPCData();
	ReceiveFastData();

	syncManager.Update(); // Update the sync manager for fast updates
}

void Server::SendRPCData()
{

}

void Server::SendFastData()
{
	SyncManager::SyncDataMap fastUpdateMap;
	syncManager.EncodeFastSyncMap(fastUpdateMap); // Encode sync data for the connection

	unsigned int message = agk::CreateNetworkMessage();
	int updateCount = 0;

	for (auto& it : fastUpdateMap)
	{
		SyncObj::SyncUUID uuid = it.first; // Get the UUID of the sync object
		std::vector<uint8_t>& data = it.second; // Get the encoded data for the sync object

		if (!data.empty())
			updateCount++;
	}
	agk::AddNetworkMessageInteger(message, updateCount); // Add the number of obj updates to the network message

	for (auto& it : fastUpdateMap)
	{
		SyncObj::SyncUUID uuid = it.first; // Get the UUID of the sync object
		std::vector<uint8_t>& data = it.second; // Get the encoded data for the sync object

		if (data.empty())
			continue; // Skip if there is no data to send

		agk::AddNetworkMessageString(message, uuid.c_str()); // Add the UUID to the network message
		agk::AddNetworkMessageInteger(message, (int)data.size()); // 

		for (int i = 0; i < data.size(); i++)
		{
			agk::AddNetworkMessageByte(message, data[i]); // Add each byte of the encoded data to the network message
		}
	}

	agk::SendUDPNetworkMessage(fastListener, message, serverAddress, serverPort + 1); // Send the network message to the server
}

void Server::ReceiveFastData()
{
	//Receive fast data from the fast listener
	unsigned int message = agk::GetUDPNetworkMessage(fastListener);

	while (message != 0)
	{
		SyncManager::SyncDataMap fastUpdateMap;
		int ind = 0;
		int updateCount = agk::GetNetworkMessageInteger(message); // Get the number of updates from the message

		while (ind < updateCount)
		{
			SyncObj::SyncUUID uuid = agk::GetNetworkMessageString(message); // Get the UUID from the message
			int dataSize = agk::GetNetworkMessageInteger(message);
			std::vector<uint8_t> data; // Create a vector to hold the data

			for (int i = 0; i < dataSize; i++)
			{
				data.push_back(agk::GetNetworkMessageByte(message)); // Extract the data from the message
			}

			agk::DeleteNetworkMessage(message); // Delete the message to free memory
			fastUpdateMap.insert(std::make_pair(uuid, data));
			ind++;
		}

		syncManager.DecodeFastSyncMap(fastUpdateMap); // Decode the received fast sync data into the sync manager
		message = agk::GetUDPNetworkMessage(fastListener); // Get the next message
	}
}