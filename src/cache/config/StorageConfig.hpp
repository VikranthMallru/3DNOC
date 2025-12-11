#include <string>
#include "types.hpp"

#ifndef STORAGECONFIG_H
#define STORAGECONFIG_H
class c_StorageConfig	{
public:
    std::string s_name;
    t_Address s_size;
    t_Address s_offset;
	t_Size s_maxReadPort; 
	t_Size s_maxWritePort;
    t_Credit s_readReqQSize;
	t_Credit s_writeReqQSize;
    t_Time s_fixedLatency;
    t_Time s_storageTimeout;
    c_StorageConfig(std::string p_storageConfigFile);
};

#endif