#include "SimConfig.hpp"
#include "LogConfig.hpp"
#include "types.hpp"
#include "IniReader.hpp"
#include <iostream>
#include <sstream> 
#include <stdlib.h>
#include <assert.h>
#include<unistd.h>
#include<cassert>

#define _POSIX_SOURCE

template <typename T>
void f_splitString(std::string &p_input, char p_delimiter, T p_arr[], int &p_index)  {
    std::istringstream l_stream(p_input); 
    string l_token; 
    while (getline(l_stream, l_token, p_delimiter)) { 
        p_arr[p_index++] = (T)atoi(l_token.c_str()); 
    }
}

c_SimConfig::c_SimConfig(std::string p_simConfigFile)    {
    try {
        c_INIReader *l_iniReader = new c_INIReader(p_simConfigFile);
        s_numCores = l_iniReader->GetInteger("Sim","NumCore",1);
        s_activeCores = new bool[s_numCores];
        s_numMemoryChannels = l_iniReader->GetInteger("Sim","NumMemChannels",1);
        s_activeMem = new bool[s_numMemoryChannels];
        s_numNoCChannels = l_iniReader->GetInteger("Sim","NumNoCChannels",1);
        m_maxInstFetched = l_iniReader->GetInteger("Sim","MaxInst",5);
        s_preloadLevel = l_iniReader->GetInteger("Sim","PreloadLevel",1);
        s_maxPrivateCacheTypes = l_iniReader->GetInteger("Sim","MaxPrivateCacheType",1);
        s_path = l_iniReader->Get("Sim","Path","");
        std::string l_logConfig = s_path + l_iniReader->Get("Sim","LogConfig","");
        s_logConfig = new c_LogConfig(l_logConfig);
    
        s_logPath = l_iniReader->Get("Sim","LogPath","");
        s_nocConfigFile = l_iniReader->Get("Hierarchy","TopologyStruct","");
        s_numNodePerChannel = new t_NodeId[s_numNoCChannels];
    	s_routingAlgorithmPerChannel = new t_RoutingAlgorithm[s_numNoCChannels];
        s_numRepeatersPerChannel = new t_NodeId[s_numNoCChannels];
        std::string l_activeCores = l_iniReader->Get("Sim","ActiveCores","");
        std::string l_activeMem = l_iniReader->Get("Sim", "ActiveMem", "");
        std::string l_numNodesChannel = l_iniReader->Get("Sim","NumNodesPerChannel","");
        std::string l_numRepeatersPerChannel = l_iniReader->Get("Sim","NumRepeatersPerChannel","");
        std::string l_routingAlgorithmPerChannel = l_iniReader->Get("Sim","RoutingAlgoPerChannel","");
        int l_index1 = 0;
        int l_index2 = 0;
        int l_index3 = 0;
        int l_index4 = 0;
        int l_index5 = 0;
        f_splitString<t_NodeId>(l_numNodesChannel,',', s_numNodePerChannel, l_index1);
        f_splitString<t_RoutingAlgorithm>(l_routingAlgorithmPerChannel,',', (t_RoutingAlgorithm*) s_routingAlgorithmPerChannel, l_index2);
        f_splitString<t_NodeId>(l_numRepeatersPerChannel,',',s_numRepeatersPerChannel,l_index3);
        f_splitString<bool>(l_activeCores,',',s_activeCores,l_index4);
        f_splitString<bool>(l_activeMem,',',s_activeMem,l_index5);
        
        s_traceFile = new std::string[s_numCores];
        s_storageConfigFile = new std::string[s_numMemoryChannels];
        s_coreConfigFile = new std::string[s_numCores];
        for(t_Size l_core = 0; l_core < s_numCores; l_core++)   {
            s_traceFile[l_core] = s_path + l_iniReader->Get("Trace","CoreTrace_" + std::to_string(l_core),"");
            s_coreConfigFile[l_core] = s_path + l_iniReader->Get("Hierarchy","CoreCFG_" + std::to_string(l_core),"");
            std::string l_key = "Cache_" + std::to_string(l_core) + "_L1D";
            std::string l_fileName = l_iniReader->Get("Hierarchy",l_key,"");
            l_key = "Cache_" + std::to_string(0) + "_L1I";
            l_fileName = l_iniReader->Get("Hierarchy",l_key,"");
            l_key = "Cache_" + std::to_string(0) + "_L2";
            l_fileName = l_iniReader->Get("Hierarchy",l_key,"");
        }
        for(t_Size l_memoryChannel = 0; l_memoryChannel < s_numMemoryChannels; l_memoryChannel++)    {
            s_storageConfigFile[l_memoryChannel] = s_path + l_iniReader->Get("Hierarchy","MM_" + std::to_string(l_memoryChannel),"");
        }
        
        s_statsFileName = s_path + l_iniReader->Get("Sim","StatsFile","");
        s_logFile = s_logPath + l_iniReader->Get("Sim","LogFile","");
        s_addressLogFile = s_logPath + l_iniReader->Get("Sim","AddressLogFile","");
        s_traceConfigFile = s_path + l_iniReader->Get("Sim","TraceConfigFile","");
        s_preload = l_iniReader->GetBoolean("Sim","Preload",false);
        s_testgen = l_iniReader->GetBoolean("Sim","TestGen",false);
        s_packetPoolSize = l_iniReader->GetInteger("Sim","PacketPoolSize",1);
        s_transactionPoolSize = l_iniReader->GetInteger("Sim","TransactionPoolSize",1);
        s_fixedLatPktPoolSize = l_iniReader->GetInteger("Sim","FixedLatencyPacketPoolSize",1);
        s_simTime = l_iniReader->GetInteger("Sim","SimTime",100);
        
        s_lsPortNo = l_iniReader->GetBoolean("Sim","LSPortNo",0);
        s_nocTimeout = l_iniReader->GetInteger("Sim","NOCTimeout",10000);
        delete l_iniReader;
    }
    catch(const std::exception& e)    {
        std::cerr << e.what() << '\n';
        exit(-1);
    }
}

c_SimConfig::~c_SimConfig()  {
    delete[] s_activeCores;
    delete[] s_traceFile;
    delete[] s_storageConfigFile;
    delete[] s_numNodePerChannel;
    delete[] s_routingAlgorithmPerChannel;
    delete[] s_numRepeatersPerChannel;
    delete[] s_coreConfigFile;
}
