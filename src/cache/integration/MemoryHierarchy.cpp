#include "MemoryHierarchy.hpp"
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
#include "Statistics.hpp"
#include "Topology.hpp"
#include "NetworkInputInterface.hpp"
#include "NetworkOutputInterface.hpp"
#include "MultiCacheNetworkInterface.hpp"
#include "CacheSimulator.hpp"
#include <assert.h>

c_MemoryHierarchy::c_MemoryHierarchy(c_CacheSimulator *p_cacheSim,c_SimConfig *p_simConfig)  {
    try    {
        m_simConfig = p_simConfig;
        m_cacheSim = p_cacheSim;
        m_numCores = p_simConfig->s_numCores;
        m_numMemoryChannels = p_simConfig->s_numMemoryChannels;
        m_threadController = new c_ThreadController();
        m_topology = new c_Topology(m_simConfig);
        m_threadController->f_addToThreads(m_topology);
        f_createMainMemory();
        f_createCore();
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error creating hierarchy\n';
    }
}

c_MemoryHierarchy::~c_MemoryHierarchy()  {
    try {
        for(t_Size l_core = 0; l_core < m_numCores; l_core++)  {
            delete m_lsu[l_core];
            delete m_fu[l_core];
            delete m_core[l_core];
            delete m_coreConfig[l_core];
        }
        for(t_Size l_memoryChannels = 0; l_memoryChannels < m_numMemoryChannels; l_memoryChannels++)  {
            delete m_mainMemory[l_memoryChannels];
            delete m_storageConfig[l_memoryChannels];
        }
        delete[] m_fu;
        delete[] m_lsu;
        delete[] m_core;
        delete[] m_mainMemory;
        delete[] m_coreConfig;
        delete[] m_storageConfig;
        delete m_threadController;
        delete m_topology;

        for(t_CoreID l_core = 0; l_core < m_simConfig->s_numCores; l_core++)    {
                delete m_multiCacheNoCInterface[l_core];
        }
        delete m_multiCacheNoCInterface;
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error deleting hierarchy\n';
    }
}

void c_MemoryHierarchy::f_preload()    {
    if(m_simConfig->s_preload)  {
        m_core[0]->f_warmUp();
    }        
}

void c_MemoryHierarchy::f_createCore()  {
    try {
        m_core = new c_TraceReader*[m_numCores];
        m_coreConfig = new c_CoreConfig*[m_numCores];
        m_lsu = new c_LoadStoreInterface*[m_numCores];
        m_fu = new c_FetchInterface*[m_numCores];
        for(t_Size l_core = 0; l_core < m_numCores; l_core++)  {
            m_coreConfig[l_core] = new c_CoreConfig(l_core, m_simConfig->s_coreConfigFile[l_core]);
            m_lsu[l_core] = new c_LoadStoreInterface(m_coreConfig[l_core]);
            m_fu[l_core] = new c_FetchInterface(this,m_simConfig, m_coreConfig[l_core]);
            std::cout<<"Adding core module: "<<m_simConfig->s_coreConfigFile[l_core]<<std::endl;
            std::cout<<"Adding trace file: "<<m_simConfig->s_traceFile[l_core]<<std::endl;
            m_core[l_core] = new c_TraceReader(m_simConfig,l_core,"Core_" + std::to_string(l_core),
                m_simConfig->s_traceFile[l_core], m_fu[l_core], m_lsu[l_core], this, m_mainMemory[l_core], m_simConfig->s_preload, m_simConfig->s_preloadLevel);
            m_fu[l_core]->f_setCore(m_core[l_core]);
            m_threadController->f_addToThreads(m_core[l_core]);
        }
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error creating cores\n';
    }
}

void c_MemoryHierarchy::f_createMainMemory()    {
    try {
        m_mainMemory = new c_FixedLatencyStorage*[m_numMemoryChannels];
        m_storageConfig = new c_StorageConfig*[m_numMemoryChannels];
        for(t_Size l_memoryChannels = 0; l_memoryChannels < m_numMemoryChannels; l_memoryChannels++)  {
            std::cout<<"Adding memory module: "<<m_simConfig->s_storageConfigFile[l_memoryChannels]<<std::endl;
            m_storageConfig[l_memoryChannels] = new c_StorageConfig(m_simConfig->s_storageConfigFile[l_memoryChannels]);
            m_mainMemory[l_memoryChannels] = new c_FixedLatencyStorage("MainMemory_" + std::to_string(l_memoryChannels),m_storageConfig[l_memoryChannels], l_memoryChannels, m_simConfig);
            m_threadController->f_addToThreads(m_mainMemory[l_memoryChannels]);
        }
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error creating memory\n';
    }
}

void c_MemoryHierarchy::f_bindPorts(c_MasterPort *p_masterPort, c_SlavePort *p_slavePort)   {
    p_masterPort->f_bind(p_slavePort);
    p_slavePort->f_bind(p_masterPort);
    std::cout<<p_masterPort->f_toString()<<" <-> "<<p_slavePort->f_toString()<<std::endl;
    std::fflush(stdout);
}

void c_MemoryHierarchy::f_linkMainMemoryInterconnect()  {
    try {
        c_NetworkOutputInterface *l_outputInterface = m_topology->f_getOutputInterface();
        c_NetworkInputInterface *l_inputInterface = m_topology->f_getInputInterface();
        for(t_Size l_memoryChannels = 0; l_memoryChannels < m_numMemoryChannels; l_memoryChannels++)  {
            std::cout<<"Binding ports: NOC-MM Memory Controller: "<<l_memoryChannels<<std::endl;
            t_PortNo l_interfacePortNo = 2 * m_simConfig->s_numNodePerChannel[0] + l_memoryChannels;
            f_bindPorts(l_outputInterface->f_getResponsePort(E_MISS,l_interfacePortNo),m_mainMemory[l_memoryChannels]->f_getRequestPort(E_CORE_READ));
            f_bindPorts(l_outputInterface->f_getResponsePort(E_VICTIM,l_interfacePortNo),m_mainMemory[l_memoryChannels]->f_getRequestPort(E_CORE_WRITE));
            f_bindPorts(l_outputInterface->f_getResponsePort(E_PROBE_RESP_SEND,l_interfacePortNo),m_mainMemory[l_memoryChannels]->f_getRequestPort(E_PROBE_RESP));
            f_bindPorts(m_mainMemory[l_memoryChannels]->f_getResponsePort(E_READ_RESP),l_inputInterface->f_getRequestPort(E_FILL,l_interfacePortNo));
            f_bindPorts(m_mainMemory[l_memoryChannels]->f_getResponsePort(E_PROBE_REQ_SEND),l_inputInterface->f_getRequestPort(E_PROBE_REQ,l_interfacePortNo));
        }
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error creating link between NOC and memory\n';
    }        
}

t_Size c_MemoryHierarchy::f_getNumCores(){
    return m_numCores;
}

t_Size c_MemoryHierarchy::f_getNumMemoryChannels(){
    return m_numMemoryChannels;
}

c_Controller** c_MemoryHierarchy::f_getCores(){
    return (c_Controller**) m_core;
}

c_Controller** c_MemoryHierarchy::f_getMainMemory(){
    return (c_Controller**) m_mainMemory;
}

void c_MemoryHierarchy::f_warmUp(c_Transaction *p_transaction)  {
    try {
        t_Size l_level = p_transaction->f_getInstallLevel();
        t_Size l_core = p_transaction->f_getRequestingCore();
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error doing warm-up\n';
    }
}

void c_MemoryHierarchy::f_finishSimulation()    {
    delete m_cacheSim;
}

void c_MemoryHierarchy::f_displayStats()    {
    c_Statistics::f_displayStats();
    for(t_Size l_core = 0; l_core < m_numCores; l_core++)  {
        m_core[l_core]->f_displayStats();
    }
}

c_CoreConfig *c_MemoryHierarchy::f_getCoreConfig(t_CoreID p_coreId)    {
    return m_coreConfig[p_coreId];
}

void c_MemoryHierarchy::f_start() {
    f_portCheck();
    m_threadController->f_start();
}

c_Topology* c_MemoryHierarchy::f_getTopology() {
    return m_topology;
}

c_SimConfig* c_MemoryHierarchy::f_getSimConfig()   {
    return m_simConfig;
}
