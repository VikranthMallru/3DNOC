#include "types.hpp"
#include "SimConfig.hpp"
#include <string>

#ifndef MEMORYHIERARCHY_H
#define MEMORYHIERARCHY_H

class c_TraceReader;
class c_Controller;
class c_LoadStoreInterface;
class c_FetchInterface;
class c_CacheController;
class c_FixedLatencyStorage;
class c_ThreadController;
class c_StorageConfig;
class c_CacheConfig;
class c_CoreConfig;
class c_Transaction;
class c_Statistics;
class c_MasterPort;
class c_SlavePort;
class c_Topology;
class c_PrefetchController;
class c_MultiCacheNetworkInterface;
class c_CacheSimulator;

class c_MemoryHierarchy   {
protected:
    t_Size m_numCores;
    t_Size m_numMemoryChannels;

    c_SimConfig *m_simConfig;
    c_LoadStoreInterface **m_lsu;
    c_FetchInterface **m_fu;

    c_TraceReader **m_core;
    c_Controller **m_tlb;

    c_Topology *m_topology;
    
    c_FixedLatencyStorage **m_mainMemory;
    c_ThreadController *m_threadController;

    c_CoreConfig **m_coreConfig;

    c_StorageConfig **m_storageConfig;
    t_Counter m_cacheIdGenerator;
    c_CacheSimulator *m_cacheSim;
    c_MultiCacheNetworkInterface **m_multiCacheNoCInterface;

    static t_RequestType f_getRequestTypeFromChannel(t_Size p_numChannel);
    static t_ResponseType f_getResponseTypeFromChannel(t_Size p_numChannel);
public:
    c_MemoryHierarchy(c_CacheSimulator *p_cacheSim,c_SimConfig *p_simConfig);
    ~c_MemoryHierarchy();
    void f_createCore();
    void f_createMainMemory();
    void f_linkMainMemoryInterconnect();
    void f_bindPorts(c_MasterPort *p_masterPort, c_SlavePort *p_slavePort);
    t_Size f_getNumCores();
    t_Size f_getNumMemoryChannels();
    c_Controller** f_getCores();
    c_Controller** f_getMainMemory();
    c_CoreConfig *f_getCoreConfig(t_CoreID p_coreId);
    c_SimConfig* f_getSimConfig();
    c_Topology* f_getTopology();

    void f_displayStats();
    void f_warmUp(c_Transaction *p_transaction);
    void f_preload();
    void f_start();
    void f_finishSimulation();

    virtual void f_createCaches() = 0;
    virtual void f_linkCoreAndCache() = 0;
    virtual void f_linkCacheInterconnect() = 0;
    virtual void f_linkCoreInterconnect() = 0;
    virtual void f_createHierarchy() = 0;
    virtual void f_resetClock() = 0;
    virtual void f_portCheck() = 0;
};

#endif
