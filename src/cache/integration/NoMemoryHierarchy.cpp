#include "NoMemoryHierarchy.hpp"
#include "ThreadController.hpp"
#include "types.hpp"
#include "StorageConfig.hpp"
#include "Controller.hpp"
#include "FixedLatencyStorage.hpp"
#include "LoadStoreInterface.hpp"
#include "FetchInterface.hpp"
#include "CoreConfig.hpp"
#include "MasterPort.hpp"
#include "SlavePort.hpp"
#include "TraceReader.hpp"
#include "Transaction.hpp"
#include "Topology.hpp"
#include "NetworkInputInterface.hpp"
#include "NetworkOutputInterface.hpp"
#include "MultiCacheNetworkInterface.hpp"
#include "CacheSimulator.hpp"
#include <assert.h>

c_NoMemoryHierarchy::c_NoMemoryHierarchy(c_CacheSimulator *p_cacheSim,c_SimConfig *p_simConfig) : c_MemoryHierarchy(p_cacheSim,p_simConfig)  {
    f_createMultiCacheNetwrokInterface();
}

c_NoMemoryHierarchy::~c_NoMemoryHierarchy()  {}

void c_NoMemoryHierarchy::f_createCaches(){}

void c_NoMemoryHierarchy::f_createMultiCacheNetwrokInterface()    {
    try {
        m_multiCacheNoCInterface = new c_MultiCacheNetworkInterface*[m_numCores];
        for(t_Size l_core = 0; l_core < m_numCores; l_core++)  {
            m_multiCacheNoCInterface[l_core] = new c_MultiCacheNetworkInterface("IFLSNoCInterface" + l_core,l_core,m_simConfig,m_coreConfig[l_core]);
            m_threadController->f_addToThreads(m_multiCacheNoCInterface[l_core]);
        }
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error creating caches\n';
    }
}


void c_NoMemoryHierarchy::f_linkCoreAndCache()    {}

void c_NoMemoryHierarchy::f_linkCacheInterconnect()  {}

void c_NoMemoryHierarchy::f_linkCoreInterconnect()  {
    try {
        c_NetworkOutputInterface *l_outputInterface = m_topology->f_getOutputInterface();
        c_NetworkInputInterface *l_inputInterface = m_topology->f_getInputInterface();
        for(t_Size l_core = 0; l_core < m_numCores; l_core++)  {
            std::cout<<"Binding ports: LSU/FU-NOC Core"<<l_core<<std::endl;
            std::cout<<"Binding ports: LSU Core: "<<l_core<<" LSU-MCNI "<<std::endl;
            f_bindPorts(m_lsu[l_core]->f_getReadReqPort(),m_multiCacheNoCInterface[l_core]->f_getRequestPort(E_CORE_READ,m_simConfig->s_lsPortNo));
            f_bindPorts(m_lsu[l_core]->f_getWriteReqPort(),m_multiCacheNoCInterface[l_core]->f_getRequestPort(E_CORE_WRITE,m_simConfig->s_lsPortNo));
            f_bindPorts(m_multiCacheNoCInterface[l_core]->f_getResponsePort(E_PROBE_REQ_SEND,m_simConfig->s_lsPortNo),m_lsu[l_core]->f_getProbeReqPort());
            f_bindPorts(m_multiCacheNoCInterface[l_core]->f_getResponsePort(E_READ_RESP,m_simConfig->s_lsPortNo),m_lsu[l_core]->f_getReadRespPort());
            f_bindPorts(m_lsu[l_core]->f_getWriteDataPort(),m_multiCacheNoCInterface[l_core]->f_getRequestPort(E_PROBE_RESP,m_simConfig->s_lsPortNo));
            std::cout<<"Binding ports: FU Core: "<<l_core<<" FU-MCNI "<<std::endl;
            f_bindPorts(m_fu[l_core]->f_getReadReqPort(),m_multiCacheNoCInterface[l_core]->f_getRequestPort(E_CORE_READ,M_FU_PORT));
            f_bindPorts(m_fu[l_core]->f_getWriteReqPort(),m_multiCacheNoCInterface[l_core]->f_getRequestPort(E_CORE_WRITE,M_FU_PORT));
            f_bindPorts(m_multiCacheNoCInterface[l_core]->f_getResponsePort(E_PROBE_REQ_SEND,M_FU_PORT),m_fu[l_core]->f_getProbeReqPort());
            f_bindPorts(m_multiCacheNoCInterface[l_core]->f_getResponsePort(E_READ_RESP,M_FU_PORT),m_fu[l_core]->f_getReadRespPort());
            f_bindPorts(m_fu[l_core]->f_getWriteDataPort(),m_multiCacheNoCInterface[l_core]->f_getRequestPort(E_PROBE_RESP,M_FU_PORT));
            std::cout<<"Binding ports: MCNI-NOC Core"<<l_core<<std::endl;
            f_bindPorts(l_outputInterface->f_getResponsePort(E_READ_RESP,l_core),m_multiCacheNoCInterface[l_core]->f_getRequestPort(E_FILL,0));
            f_bindPorts(l_outputInterface->f_getResponsePort(E_PROBE_REQ_SEND,l_core),m_multiCacheNoCInterface[l_core]->f_getRequestPort(E_PROBE_REQ,0));
            f_bindPorts(m_multiCacheNoCInterface[l_core]->f_getResponsePort(E_MISS,0),l_inputInterface->f_getRequestPort(E_CORE_READ,l_core));
            f_bindPorts(m_multiCacheNoCInterface[l_core]->f_getResponsePort(E_VICTIM,0),l_inputInterface->f_getRequestPort(E_CORE_WRITE,l_core));
            f_bindPorts(m_multiCacheNoCInterface[l_core]->f_getResponsePort(E_PROBE_RESP_SEND,0),l_inputInterface->f_getRequestPort(E_PROBE_RESP,l_core));    
        }
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error creating link between caches and NOC\n';
    }        
}

void c_NoMemoryHierarchy::f_resetClock()  {
    try {
        for(t_Size l_memoryChannels = 0; l_memoryChannels < m_numMemoryChannels; l_memoryChannels++)  {
            m_mainMemory[l_memoryChannels]->f_resetClock();
        }
        for(t_Size l_core = 0; l_core < m_numCores; l_core++)  {
            m_lsu[l_core]->f_resetClock();
            m_fu[l_core]->f_resetClock();
            m_multiCacheNoCInterface[l_core]->f_resetClock();
        }
        m_topology->f_resetClock();
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error resetting clock\n';
    }
}

void c_NoMemoryHierarchy::f_createHierarchy() {
    f_linkCoreInterconnect();
    f_linkMainMemoryInterconnect();
    m_topology->f_connect();
}

void c_NoMemoryHierarchy::f_portCheck()   {
    try {
        for(t_Size l_memoryChannels = 0; l_memoryChannels < m_numMemoryChannels; l_memoryChannels++)  {
            m_mainMemory[l_memoryChannels]->f_portCheck();
        }
        for(t_Size l_core = 0; l_core < m_numCores; l_core++)  {
            m_lsu[l_core]->f_portCheck();
            m_fu[l_core]->f_portCheck();
            m_multiCacheNoCInterface[l_core]->f_portCheck();
        }
        m_topology->f_portCheck();
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error resetting clock\n';
    }
}
