#ifndef _H_UDPSYNC
#define _H_UDPSYNC

#include <string>
#include <vector>

#include "AGK-Reliable-UDP/Util.h"

class UDPSync {
public:
    typedef std::string SyncUUID;

    virtual ~UDPSync() {}
    virtual void SyncEncode(std::vector<uint8_t>& outData) const = 0;
    virtual void SyncDecode(std::vector<uint8_t>& inData) = 0;
    virtual void SyncUpdate() = 0;
    virtual SyncUUID GetUUID() const {
        return syncUUID; // Default implementation
    }

protected:
    SyncUUID syncUUID; // UUID storage for derived classes
    // Constructor to automatically generate a UUID
    UDPSync() : syncUUID(get_uuid()) {}
};

#endif



