#include "SyncManager.h"

SyncManager::SyncManager(Authority auth) 
{
	authority = auth;
}

SyncManager::~SyncManager()
{
	syncMap.clear();
}

void SyncManager::SyncUpdate()
{
	//Tick sync update
	for (auto& pair : syncMap) 
	{
		pair.second.syncObj->SyncUpdate(); // Call the update method on each SyncObj object
	}
}

void SyncManager::AddSync(SyncObj::SyncUUID uuid, SyncObj* syncObj, Authority auth)
{
	if (syncMap.find(uuid) == syncMap.end()) 
	{
		syncMap.emplace(uuid,SyncStruct(syncObj, auth)); // Add the new SyncObj object
	}
}

void SyncManager::RemoveSync(SyncObj::SyncUUID uuid)
{
	auto it = syncMap.find(uuid);
	
	if (it != syncMap.end()) 
	{
		syncMap.erase(it); // Remove from the map
	}
}

void SyncManager::EncodeSyncData(SyncDataMap& updateMap)
{
	//Encode for every object we have authority over
	for (auto& it : syncMap) 
	{
		if (authority >= it.second.authority) // Only encode if we have authority over the object
		{
			it.second.syncObj->SyncEncode(updateMap[it.first]); // Encode the sync data for each SyncObj object
		}
	}
}

void SyncManager::DecodeSyncData(SyncDataMap& updateMap)
{
	
}