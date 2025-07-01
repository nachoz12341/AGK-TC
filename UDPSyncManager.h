#ifndef _H_UDPSYNC_MANAGER
#define _H_UDPSYNC_MANAGER

#include <map>
#include "UDPSync.h"

class UDPSyncManager {
public:
	typedef enum Authority {
		Client,
		Server
	}Authority;

	UDPSyncManager(Authority auth);
	~UDPSyncManager();
	void Update();	//Ticks all sync updates
	void AddSync(UDPSync::SyncUUID uuid, UDPSync* syncObj, Authority auth);
	void RemoveSync(UDPSync::SyncUUID uuid);

private:
	typedef struct SyncStruct {
		UDPSync* syncObj;
		Authority authority; // Authority of the obj
		SyncStruct(UDPSync* _syncObj, Authority _auth) : syncObj(_syncObj), authority(_auth) {

		}
	} SyncStruct;

	//Key's being a pointer need to be compared by value
	struct cmp_str {
		bool operator()(const char* a, const char* b) const {
			return std::strcmp(a, b) < 0;
		}
	};

	typedef std::map<UDPSync::SyncUUID, SyncStruct, cmp_str> SyncMap;
	SyncMap syncMap; // Map of UUID to UDPSync objects
	Authority authority;

	void EncodeSyncData(); // Function to encode sync data for owned objects
	void DecodeSyncData(); // Function to decode sync data received from the network

};

#endif 
