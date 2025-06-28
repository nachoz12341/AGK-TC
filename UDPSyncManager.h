#ifndef _H_UDPSYNC_MANAGER
#define _H_UDPSYNC_MANAGER

#include <map>
#include "UDPSync.h"

class UDPSyncManager {
public:
	UDPSyncManager();
	~UDPSyncManager();
	void Update();
	void AddSync(UDPSync::SyncUUID uuid, UDPSync* syncObj);
	void RemoveSync(UDPSync::SyncUUID uuid);

private:
	//Key's being a pointer need to be compared by value
	struct cmp_str {
		bool operator()(const char* a, const char* b) const {
			return std::strcmp(a, b) < 0;
		}
	};

	typedef std::map<UDPSync::SyncUUID, UDPSync*, cmp_str> SyncMap;


}

#endif 
