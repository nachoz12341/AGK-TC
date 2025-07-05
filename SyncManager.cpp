#include "SyncManager.h"

#include "Player.h"

SyncManager::SyncManager(Authority auth) 
{
	authority = auth;
	syncMap.emplace(GetUUID(), SyncStruct(this, auth)); //Add manager to the sync map so it can sync object creation and deletion
}

SyncManager::~SyncManager()
{
	syncMap.clear();
}

void SyncManager::Update()
{
	//Tick sync update
	for (auto& pair : syncMap) 
	{
		pair.second.syncObj->SyncFastUpdate(); // Call the update method on each SyncObj object
	}
}

void SyncManager::AddSyncObj(SyncObj::SyncUUID uuid, SyncObj* syncObj, Authority auth)
{
	if (syncMap.find(uuid) == syncMap.end()) 
	{
		syncMap.emplace(uuid,SyncStruct(syncObj, auth)); // Add the new SyncObj object
		
		std::vector<uint8_t>data;
		EncodeValue(data, uuid); // Add the authority to the data
		EncodeValue(data, (uint8_t)auth); // Add the authority to the data
		EncodeValue(data, (uint8_t)syncObj->GetSyncObjectID()); // Add the object ID to the data

		AddRpc(SyncObj::RpcMessage{ RPC::CREATE_OBJ, data}); // Let others know we deleted the object
	}
}

void SyncManager::RemoveSyncObj(SyncObj::SyncUUID uuid)
{
	auto it = syncMap.find(uuid);
	
	if (it != syncMap.end()) 
	{
		syncMap.erase(it); // Remove from the map
		std::vector<uint8_t>data;
		EncodeValue(data, uuid); // Add the authority to the data
		AddRpc(SyncObj::RpcMessage{ RPC::DELETE_OBJ, data }); // Let others know we deleted the object
	}
}

/*
** Fast Sync data
*/
//For every object we have authority over, encode the sync data
void SyncManager::EncodeFastSyncMap(SyncDataMap& updateMap)
{
	//Encode for every object we have authority over
	for (auto& it : syncMap)
	{
		SyncUUID uuid = it.first;
		SyncStruct& syncStruct = it.second;

		if (authority >= syncStruct.authority) // Only encode if we have authority over the object
		{
			syncStruct.syncObj->SyncFastEncode(updateMap[uuid]); // Encode the sync data for each SyncObj object
		}
	}
}

//For every uuid in update map, decode the sync data
void SyncManager::DecodeFastSyncMap(SyncDataMap& updateMap)
{
	for (auto& it : updateMap)
	{
		SyncUUID uuid = it.first;
		std::vector<uint8_t>& data = it.second;


		auto syncPair = syncMap.find(uuid); // Get the sync pair from the map

		if (syncPair == syncMap.end())
			continue; // If the uuid is not in the sync map, skip it

		syncPair->second.syncObj->SyncFastDecode(data); //Decode the sync data for each SyncObj object
	}
}


/*
**	RPC Syncing
*/

//Every object encode the rpc outqueue into the update map
void SyncManager::EncodeRPCMap(SyncDataMap& updateMap)
{
	for (auto& it : syncMap)
	{
		SyncUUID uuid = it.first;
		SyncStruct& syncStruct = it.second;

		syncStruct.syncObj->SyncRPCEncode(updateMap[uuid]); // Encode the sync data for each SyncObj object
	}
}

//For every uuid in update map, decode the rpc data and act on it
void SyncManager::DecodeRPCMap(SyncDataMap& updateMap)
{
	for (auto& it : updateMap)
	{
		SyncUUID uuid = it.first;
		std::vector<uint8_t>& data = it.second;


		auto syncPair = syncMap.find(uuid); // Get the sync pair from the map

		if (syncPair == syncMap.end())
			continue; // If the uuid is not in the sync map, skip it

		syncPair->second.syncObj->SyncRPCDecode(data);	//Fill the rpc queue with the data
		syncPair->second.syncObj->SyncRPCUpdate();			//Act on rpc messages
	}
}

//Personal function for rpc manager's internal sync obj
void SyncManager::SyncRPCUpdate()
{
	while (!inRpcQueue.empty())
	{
		RpcMessage &rpc = inRpcQueue.front();
		inRpcQueue.pop();

		switch (rpc.id)
		{
			case CREATE_OBJ:
			{
				SyncUUID uuid;
				DecodeValue(rpc.data,uuid);

				Authority auth;	//Get the authority byte from the data
				DecodeValue(rpc.data, auth); 

				SyncObjectID objID; // Get the object ID from the first byte of data
				DecodeValue(rpc.data, objID); 

				SyncObj* syncObj = nullptr;

				switch(objID)
				{
					case SyncObjectID::PLAYER:
					{
						syncObj = new Player(nullptr, 0.0f, 0.0f); // Create a new Player object
						syncObj->SetUUID(uuid); // Set the UUID for the player
						break;
					}

					default:
					{
						break;
					}
				}

				syncMap.emplace(uuid, SyncStruct(syncObj, auth)); // Add the new SyncObj object
				break;
			}

			case DELETE_OBJ:
			{
				RemoveSyncObj(SyncUUID(rpc.data.begin(), rpc.data.end())); // Remove the sync object from the map
				break;
			}
		}
	}
}
