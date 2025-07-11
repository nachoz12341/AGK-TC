#ifndef _H_SYNC_MANAGER
#define _H_SYNC_MANAGER

#include <map>
#include "SyncObj.h"
#include "AGK-Reliable-UDP/RUDPListener.h"

class SyncManager: public SyncObj {
public:
	//Currently sent as uint8_t, but can be changed to int if needed
	enum Authority {
		Client,
		Server
	};
	
	typedef std::map<SyncObj::SyncUUID, std::vector<uint8_t>> SyncDataMap; // Map of UUID to sync data

	SyncManager(Authority auth, RUDPListener::ConnectionUUID ownerUUID);
	~SyncManager();
	void Update();	//Ticks all sync updates

	void AddSyncObj(SyncObj::SyncUUID uuid, SyncObj* syncObj, Authority auth, RUDPListener::ConnectionUUID owningConnection);
	void RemoveSyncObj(SyncObj::SyncUUID uuid);
	RUDPListener::ConnectionUUID GetOwningConnection(SyncObj::SyncUUID uuid) const;

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
		RUDPListener::ConnectionUUID ownerUUID; // Connection UUID of the owner
		SyncStruct(SyncObj* _syncObj, Authority _auth, RUDPListener::ConnectionUUID _ownerUUID) : syncObj(_syncObj), authority(_auth), ownerUUID(_ownerUUID) {

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
