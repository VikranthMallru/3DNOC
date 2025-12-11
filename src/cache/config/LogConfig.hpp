#include <string>
#include "types.hpp"

#ifndef LOGCONFIG_H
#define LOGCONFIG_H
class c_LogConfig	{
public:
    bool s_pipeView;
    bool s_transactionLog;
    bool s_addressLog;
    t_Address s_addressLogAddress;
    t_Time s_transactionLogStartCycle;
    t_Time s_transactionLogEndCycle;
    t_Time s_pipeViewStartCycle;
    t_Time s_pipeViewEndCycle;
    t_Time s_addressLogStartCycle;
    t_Time s_addressLogEndCycle;
    c_LogConfig(std::string p_logConfigFile);
};

#endif