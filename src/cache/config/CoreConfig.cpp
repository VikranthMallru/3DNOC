#include "StorageConfig.hpp"
#include "types.hpp"
#include "IniReader.hpp"
#include "CoreConfig.hpp"
#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>

c_CoreConfig::c_CoreConfig(t_CoreID p_coreId, std::string p_coreConfigFile)    {
    try {
        c_INIReader *l_iniReader = new c_INIReader(p_coreConfigFile);
        s_name = l_iniReader->Get("Basic","Name","LS");
        s_coreId = p_coreId;
        s_fetchQSize = l_iniReader->GetInteger("Queue","FetchQSize",0);
        s_loadQSize = l_iniReader->GetInteger("Queue","LoadQSize",0);
        s_storeQSize = l_iniReader->GetInteger("Queue","StoreQSize",0);
        s_robSize = l_iniReader->GetInteger("Queue","ROBSize",0);
        s_ibSize = s_fetchQSize;
        s_coreTimeout = l_iniReader->GetInteger("Basic","CoreTimeout",10000);
        s_trafficMode = l_iniReader->Get("Basic", "TrafficMode", "");
        s_seed = l_iniReader->GetInteger("Basic", "Seed", 0);
        s_param1 = l_iniReader->GetInteger("Basic", "Param1", 0);
        s_param2 = l_iniReader->GetInteger("Basic", "Param2", 0);
        delete l_iniReader;
    }
    catch(const std::exception& e)    {
        std::cerr << e.what() << '\n';
        exit(-1);
    }
}
