#ifndef _H_SYNC_MANAGER
#define _H_SYNC_MANAGER

#include <map>
#include "SyncObj.h"

class SyncManager {
public:
	typedef enum Authority {
		Client,
		Server
	}Authority;
	
	typedef std::map<SyncObj::SyncUUID, std::vector<uint8_t>> SyncDataMap; // Map of UUID to sync data

	SyncManager(Authority auth);
	~SyncManager();
	void SyncUpdate();	//Ticks all sync updates
	void AddSync(SyncObj::SyncUUID uuid, SyncObj* syncObj, Authority auth);
	void RemoveSync(SyncObj::SyncUUID uuid);

	void EncodeSyncData(SyncDataMap& updateMap); // Function to encode sync data for owned objects
	void DecodeSyncData(SyncDataMap& updateMap); // Function to decode sync data received from the network

private:
	typedef struct SyncStruct {
		SyncObj* syncObj;
		Authority authority; // Authority of the obj
		SyncStruct(SyncObj* _syncObj, Authority _auth) : syncObj(_syncObj), authority(_auth) {

		}
	} SyncStruct;

	typedef std::map<SyncObj::SyncUUID, SyncStruct> SyncMap;
	SyncMap syncMap; // Map of UUID to SyncObj objects
	Authority authority;

	
};

#endif 
