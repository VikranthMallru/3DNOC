#include <string>
#include "types.hpp"

#ifndef CORECONFIG_H
#define CORECONFIG_H
class c_CoreConfig	{
public:
    std::string s_name;
	std::string s_trafficMode;
    t_Credit s_fetchQSize;
    t_Credit s_loadQSize;
    t_Credit s_storeQSize;
    t_Credit s_robSize;
    t_Credit s_ibSize;
    t_Credit s_coreId;
    t_Time s_coreTimeout;
	t_Size s_seed;
	t_Size s_param1;
	t_Size s_param2;
    c_CoreConfig(t_CoreID p_coreId, std::string p_coreConfigFile);
};

#endif
