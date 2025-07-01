#include "UDPSyncManager.h"

UDPSyncManager::UDPSyncManager(Authority auth) 
{
	authority = auth;
}

UDPSyncManager::~UDPSyncManager()
{
	for (auto& pair : syncMap) {
		delete pair.second.syncObj; // Clean up dynamically allocated UDPSync objects
	}
	syncMap.clear();
}

void UDPSyncManager::Update()
{
	DecodeSyncData();

	//Tick sync update
	for (auto& pair : syncMap) 
	{
		pair.second.syncObj->SyncUpdate(); // Call the update method on each UDPSync object
	}

	EncodeSyncData(); // This function should encode the sync data for owned objects
}

void UDPSyncManager::AddSync(UDPSync::SyncUUID uuid, UDPSync* syncObj, Authority auth)
{
	if (syncMap.find(uuid) == syncMap.end()) 
	{
		syncMap.emplace(SyncStruct(syncObj, auth)); // Add the new UDPSync object
	}
}

void UDPSyncManager::RemoveSync(UDPSync::SyncUUID uuid)
{
	auto it = syncMap.find(uuid);
	
	if (it != syncMap.end()) 
	{
		delete it->second.syncObj; // Clean up the dynamically allocated UDPSync object
		syncMap.erase(it); // Remove from the map
	}
}

void UDPSyncManager::EncodeSyncData()
{
	//Create a vector that holds the encoded sync data for each UDPSync object
	std::map<UDPSync::SyncUUID, std::vector<uint8_t>, cmp_str> updateMap;
	

	//Encode for every object we have authority over
	for (auto& it : syncMap) 
	{
		if (authority >= it.second.authority) // Only encode if we have authority over the object
		{
			it.second.syncObj->SyncEncode(updateMap[it.first]); // Encode the sync data for each UDPSync object
		}
	}

	//Send update
}