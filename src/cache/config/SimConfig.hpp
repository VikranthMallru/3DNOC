#include <string>
#include "types.hpp"

#ifndef SIMCONFIG_H
#define SIMCONFIG_H

class c_LogConfig;

class c_SimConfig	{
public:
	t_Size s_numCores;
	t_NetworkChannel s_numNoCChannels;
	t_Size s_numMemoryChannels;
	t_Size s_maxPrivateCacheTypes;
	t_CacheLevel s_preloadLevel;
	t_PortNo s_lsPortNo;

	t_RoutingAlgorithm *s_routingAlgorithmPerChannel;
	t_NodeId *s_numNodePerChannel;
	t_NodeId *s_numRepeatersPerChannel;

	std::string s_path;
	std::string s_logPath;
  std::string *s_traceFile;
  std::string *s_storageConfigFile;
  std::string *s_coreConfigFile;
  std::string s_statsFileName;
	std::string s_logFile;
	std::string s_addressLogFile;
	std::string s_traceConfigFile;
	std::string s_nocConfigFile;
	c_LogConfig *s_logConfig;
    
	bool s_preload;
	bool s_testgen;
	bool *s_activeCores;
	bool *s_activeMem;
	t_Size s_packetPoolSize;
	t_Size s_transactionPoolSize;
	t_Size s_fixedLatPktPoolSize;
	t_Time s_simTime;
	t_Time s_nocTimeout;
	t_Size m_maxInstFetched;
	
	c_SimConfig(std::string p_simConfigFile);
	~c_SimConfig();

	bool f_isSimulatingCore(t_CoreID p_currCore);
};
#endif
