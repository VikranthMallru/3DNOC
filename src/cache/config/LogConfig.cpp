#include "LogConfig.hpp"
#include "types.hpp"
#include "IniReader.hpp"
#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>

c_LogConfig::c_LogConfig(std::string p_logConfigFile)    {
    try    {
        c_INIReader *l_iniReader = new c_INIReader(p_logConfigFile);
        s_pipeView = l_iniReader->GetBoolean("Log","PipeView",false);
        s_transactionLog = l_iniReader->GetBoolean("Log","TransactionLog",false);
        s_addressLog = l_iniReader->GetBoolean("Log","AddressLog",false);
        s_addressLogAddress = l_iniReader->GetInteger("Log","AddressLogAddress",-1);
        s_addressLogStartCycle = l_iniReader->GetInteger("Log","AddressLogStartCycle",-1);
        s_addressLogEndCycle = l_iniReader->GetInteger("Log","AddressLogEndCycle",-1);        
        s_transactionLogStartCycle = l_iniReader->GetInteger("Log","TransactionLogStartCycle",-1);
        s_transactionLogEndCycle = l_iniReader->GetInteger("Log","TransactionLogEndCycle",-1);
        s_pipeViewStartCycle = l_iniReader->GetInteger("Log","PipeViewStartCycle",-1);
        s_pipeViewEndCycle = l_iniReader->GetInteger("Log","PipeViewEndCycle",-1);
        delete l_iniReader;
    }
    catch(const std::exception& e)    {
        std::cerr << e.what() << '\n';
        exit(-1);
    }
}
