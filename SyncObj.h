#ifndef _H_SyncObj
#define _H_SyncObj

#include <string>
#include <vector>
#include <queue>

#include "Util.h"

class SyncObj {
public:
    typedef std::string SyncUUID;

    struct RpcMessage {
        uint8_t id;
        std::vector<uint8_t> data;
    };

	//Currently sending as uint8_t, but can be changed to int if needed
    enum SyncObjectID {
        DEFAULT,
        PLAYER,
    };

    virtual ~SyncObj() {};
    virtual void SyncFastUpdate() {};
    virtual void SyncFastEncode(std::vector<uint8_t>& outData) const {};
    virtual void SyncFastDecode(std::vector<uint8_t>& inData) {};
    virtual void SyncRPCUpdate() = 0;
    

    virtual SyncUUID GetUUID() const {
        return syncUUID; // Default implementation
    };

    virtual SyncObjectID GetSyncObjectID() const {
        return SyncObj::DEFAULT; // Default implementation
	};

    virtual void AddRpc(const RpcMessage& rpc) {
        outRpcQueue.push(rpc);
    };

    //Same for all SyncObj objects
    void SyncRPCEncode(std::vector<uint8_t>& outData)
    {
        if (outRpcQueue.empty())
            return; // If there are no pending RPCs, do nothing

        // Encode the # of rpc messages into the output data
        int queueSize = static_cast<int>(outRpcQueue.size());
        const uint8_t* sizePtr = reinterpret_cast<const uint8_t*>(&queueSize);
        outData.insert(outData.end(), sizePtr, sizePtr + sizeof(int));

        // Encode all pending RPCs into the output data
        while (!outRpcQueue.empty())
        {
            RpcMessage rpc = outRpcQueue.front();
            outRpcQueue.pop();

            //Push rpc id
            outData.push_back(static_cast<uint8_t>(rpc.id));

            //Push size of data
            int dataSize = static_cast<int>(rpc.data.size());
            const uint8_t* sizePtr = reinterpret_cast<const uint8_t*>(&dataSize); // Append the size as 4 bytes (int) to outData
            outData.insert(outData.end(), sizePtr, sizePtr + sizeof(int));

            //Push data
            outData.insert(outData.end(), rpc.data.begin(), rpc.data.end());
        }
    };

    void SyncRPCDecode(std::vector<uint8_t>& inData)
    {
        if (inData.size() < sizeof(int))
            return; // If the data is too small, return

        //Get how many rpc messages we have
        int queueSize = 0;
        std::memcpy(&queueSize, inData.data(), sizeof(int));
        inData.erase(inData.begin(), inData.begin() + sizeof(int));

        while (queueSize > 0)
        {
            queueSize--; // Decrement the queue size

            uint8_t rpcId = inData[0]; // Get the RPC ID from the data
            inData.erase(inData.begin()); // Remove the RPC ID from the data

            int dataSize = 0;
            std::memcpy(&dataSize, inData.data(), sizeof(int));
            inData.erase(inData.begin(), inData.begin() + sizeof(int));

            std::vector<uint8_t> rpcData(inData.begin(), inData.begin() + dataSize); // Extract the RPC data
            inData.erase(inData.begin(), inData.begin() + dataSize); // Remove the RPC data from the input data
            RpcMessage rpc = { rpcId, rpcData }; // Create a new RpcMessage

            inRpcQueue.push(rpc); // Push the RPC message to the queue
        }
    };

    void SetUUID(const SyncUUID& uuid) {
        syncUUID = uuid; // Set the UUID for the sync object
    };
protected:
    SyncUUID syncUUID; // UUID storage for derived classes
    std::queue<RpcMessage> outRpcQueue; // Queue for pending RPCs
    std::queue<RpcMessage> inRpcQueue; // Queue for pending RPCs
    SyncObj() : syncUUID(get_uuid()) {} // Constructor to automatically generate a UUID
};

#endif



