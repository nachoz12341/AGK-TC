#ifndef _H_SyncObj
#define _H_SyncObj

#include <string>
#include <vector>

#include "Util.h"

class SyncObj {
public:
    typedef std::string SyncUUID;

    virtual ~SyncObj() {}
    virtual void SyncEncode(std::vector<uint8_t>& outData) const = 0;
    virtual void SyncDecode(std::vector<uint8_t>& inData) = 0;
    virtual void SyncUpdate() = 0;
    virtual SyncUUID GetUUID() const {
        return syncUUID; // Default implementation
    }

protected:
    SyncUUID syncUUID; // UUID storage for derived classes
    // Constructor to automatically generate a UUID
    SyncObj() : syncUUID(get_uuid()) {}
};

#endif



