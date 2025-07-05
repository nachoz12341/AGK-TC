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
		serverUUID = nullptr;										// Set connectionUUID to nullptr if not connected
		reliableListener.Connect(serverAddress, serverPort);		//Continue trying to connnect		
	}
	else
	{
		serverUUID = connection; // Set the serverUUID to the connection UUID if connected
		
		//Send Data
		SendRPCData();
		SendFastData();

		//Receive data
		ReceiveRPCData();
		ReceiveFastData();
	}

	syncManager.Update(); 
}

void Client::RegisterObject(SyncObj* syncObj)
{
	syncManager.AddSyncObj(syncObj->GetUUID(), syncObj, SyncManager::Client); // Register the sync object with the sync manager
}

bool Client::IsConnected() const
{
	return serverUUID != nullptr; // Check if the client is connected to the server
}

void Client::SendRPCData()
{
	SyncManager::SyncDataMap reliableUpdateMap; // Create a map to hold sync  data updates
	syncManager.EncodeRPCMap(reliableUpdateMap);

	// Create a vector to hold the RPC data
	std::vector<uint8_t> rpcData; 

	for(auto &it: reliableUpdateMap)
	{
		SyncObj::SyncUUID uuid = it.first; // Get the UUID of the sync object
		std::vector<uint8_t>& data = it.second; // Get the encoded data for the sync object
		int dataSize = (int)data.size(); // Get the size of the encoded data
		
		if (data.empty())
			continue; // Skip if there is no data to send
		
		rpcData.insert(rpcData.end(), uuid.begin(), uuid.end()); // Append the UUID to the rpcData
		rpcData.insert(rpcData.end(), reinterpret_cast<const uint8_t*>(&dataSize), reinterpret_cast<const uint8_t*>(&dataSize) + sizeof(int));
		rpcData.insert(rpcData.end(), data.begin(), data.end()); // Append the encoded data to the rpcData
	}

	//Convert to memblock
	unsigned int memblock = agk::CreateMemblock((unsigned int)rpcData.size()); // Create a memory block to hold the RPC data
	
	for(int i = 0; i < rpcData.size(); i++)
	{
		agk::SetMemblockByte(memblock, i, rpcData[i]); // Set each byte of the memory block with the RPC data
	}

	reliableListener.SendMessage(serverUUID, memblock); // Send the encoded sync data to the server
}

void Client::SendFastData()
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

		for(int i= 0; i < data.size(); i++)
		{
			agk::AddNetworkMessageByte(message, data[i]); // Add each byte of the encoded data to the network message
		}
	}

	agk::SendUDPNetworkMessage(fastListener, message, serverAddress, serverPort+1); // Send the network message to the server
}

void Client::ReceiveRPCData()
{
	int message = reliableListener.GetMessage(serverUUID); // Get the message from the reliable listener
	
	while (message != -1)
	{
		SyncManager::SyncDataMap rpcUpdateMap;

		int ind = 0;
		while (ind < agk::GetMemblockSize(message))
		{
			SyncObj::SyncUUID uuid = agk::GetMemblockString(message, ind, 36); // Get the UUID from the message
			int dataSize = agk::GetMemblockInt(message, ind +36);
			ind += 40; // Move the index past the UUID and data size
			std::vector<uint8_t> data;

			for (int i = 0; i < dataSize; i++)
			{
				data.push_back(agk::GetMemblockByte(message, ind + i)); // Extract the data from the message
				ind++; // Increment the index
			}
			rpcUpdateMap.insert(std::make_pair(uuid, data));
		}
		

		syncManager.DecodeRPCMap(rpcUpdateMap); // Decode the received RPC data into the sync manager
		message = reliableListener.GetMessage(serverUUID); // Get the next message
	}
}

void Client::ReceiveFastData()
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