#include "RequestPool.hpp"

class c_Transaction;
class c_Packet;
class c_NetworkPacket;
class c_DirectoryPacket;
class c_FixedLatencyStoragePacket;
class c_MemoryHierarchy;
class c_SimConfig;

class c_CacheSimulator  {
    c_SimConfig *m_config;
    c_RequestPool<c_Transaction> *m_transactionPool;
    c_RequestPool<c_Packet> *m_packetPool;
    c_RequestPool<c_NetworkPacket> *m_networkPacketPool;
    c_RequestPool<c_FixedLatencyStoragePacket> *m_fixedLatPacketPool;
    c_RequestPool<c_DirectoryPacket> *m_directoryPacketPool;
    c_MemoryHierarchy *m_memoryHierarchy;
public:
    c_CacheSimulator(std::string p_simConfigFile);
    ~c_CacheSimulator();
};
