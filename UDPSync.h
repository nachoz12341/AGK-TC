#ifndef _H_UDPSYNC
#define _H_UDPSYNC

class UDPSync {
public:
    virtual ~UDPSync() {}
    virtual void SyncEncode(std::vector<uint8_t>& outData) const = 0;
    virtual void SyncDecode(std::vector<uint8_t>& inData) = 0;
    virtual void SyncUpdate() = 0;
};

#endif // !_H_UDPSYNC



