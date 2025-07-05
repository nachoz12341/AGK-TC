#ifndef _H_SYNC_MANAGER
#define _H_SYNC_MANAGER

#include <map>
#include "SyncObj.h"

class SyncManager: public SyncObj {
public:
	typedef enum Authority {
		Client,
		Server
	}Authority;
	
	typedef std::map<SyncObj::SyncUUID, std::vector<uint8_t>> SyncDataMap; // Map of UUID to sync data

	SyncManager(Authority auth);
	~SyncManager();
	void Update();	//Ticks all sync updates

	void AddSyncObj(SyncObj::SyncUUID uuid, SyncObj* syncObj, Authority auth);
	void RemoveSyncObj(SyncObj::SyncUUID uuid);

	void EncodeRPCMap(SyncDataMap& updateMap); // Function to encode sync data for owned objects
	void DecodeRPCMap(SyncDataMap& updateMap); // Function to decode sync data received from the network

	void EncodeFastSyncMap(SyncDataMap& updateMap); // Function to encode sync data for owned objects
	void DecodeFastSyncMap(SyncDataMap& updateMap); // Function to decode sync data received from the network

	/// Decodes rpcs for sync manager specific messages
	void SyncRPCUpdate() override;
private:
	typedef struct SyncStruct {
		SyncObj* syncObj;
		Authority authority; // Authority of the obj
		SyncStruct(SyncObj* _syncObj, Authority _auth) : syncObj(_syncObj), authority(_auth) {

		}
	} SyncStruct;

	enum RPC {
		CREATE_OBJ,
		DELETE_OBJ
	};

	typedef std::map<SyncObj::SyncUUID, SyncStruct> SyncMap;
	SyncMap syncMap; // Map of UUID to SyncObj objects
	Authority authority;

	
};

#endif 
