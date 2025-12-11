#include "ThreadController.hpp"
#include "StorageConfig.hpp"
#include "CoreConfig.hpp"
#include "RequestPool.hpp"
#include "NoMemoryHierarchy.hpp"
#include "TypeToString.hpp"
#include "CacheLogger.hpp"
#include "AddressLogger.hpp"
#include <iostream>
#include "Topology.hpp"
#include "CacheSimulator.hpp"

c_CacheSimulator::c_CacheSimulator(std::string p_simConfigFile)   {
    m_config = new c_SimConfig(p_simConfigFile);
    
    m_transactionPool = new c_RequestPool<c_Transaction>(m_config->s_transactionPoolSize);
    m_packetPool = new c_RequestPool<c_Packet>(m_config->s_packetPoolSize);
    m_networkPacketPool = new c_RequestPool<c_NetworkPacket>(m_config->s_packetPoolSize);
    m_directoryPacketPool = new c_RequestPool<c_DirectoryPacket>(m_config->s_packetPoolSize);
    m_fixedLatPacketPool = new c_RequestPool<c_FixedLatencyStoragePacket>(m_config->s_fixedLatPktPoolSize);
    
    c_TypeToString::load();
    c_CacheLogger::initLogger(m_config->s_logFile, m_config->s_logConfig);
    c_AddressLogger::initLogger(m_config->s_addressLogFile,m_config->s_logConfig);
    
    m_memoryHierarchy = new c_NoMemoryHierarchy(this, m_config);

    m_memoryHierarchy->f_createHierarchy();
    // if(m_config->s_testgen) {
    //     for(t_Size l_core = 0; l_core < m_config->s_numCores; l_core++) {
    //         c_TraceWritter l_writter(m_config,l_core,m_config->s_numSlices,m_config->s_path,
    //             m_config->s_l2CacheConfigFile[l_core],m_config->s_traceConfigFile,m_config->s_storageConfigFile[0],m_config->s_traceFile[l_core]);

	//         l_writter.f_generateBWTest();
    //     }
    // }
    m_memoryHierarchy->f_preload();
    std::cout<<"Starting simulation"<<std::endl;
    m_memoryHierarchy->f_start();
}

c_CacheSimulator::~c_CacheSimulator()   {
    m_memoryHierarchy->f_displayStats();
    //m_packetPool->f_checkLeak();
    //m_networkPacketPool->f_checkLeak();
    //m_fixedLatPacketPool->f_checkLeak();
    //m_transactionPool->f_checkLeak();
    delete m_fixedLatPacketPool;
    delete m_packetPool;
    delete m_networkPacketPool;
    delete m_transactionPool;
    delete m_directoryPacketPool;
    delete m_memoryHierarchy;
    std::cout<<"Stopped simulation"<<std::endl;
    exit(0);
}
