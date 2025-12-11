#include "StorageConfig.hpp"
#include "types.hpp"
#include "IniReader.hpp"
#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>

c_StorageConfig::c_StorageConfig(std::string p_storageConfigFile)    {
    try    {
        c_INIReader *l_iniReader = new c_INIReader(p_storageConfigFile);
        s_name = l_iniReader->Get("Basic","Name","Storage");;
        s_maxReadPort = l_iniReader->GetInteger("Port","ReadPorts",0);
        s_maxWritePort = l_iniReader->GetInteger("Port","WritePorts",0);
        s_readReqQSize = l_iniReader->GetInteger("Queue","ReadQSize",0);
        s_writeReqQSize = l_iniReader->GetInteger("Queue","WriteQSize",0);
        s_fixedLatency  = l_iniReader->GetInteger("Latency","FixedLatency",0);
        s_size  = l_iniReader->GetInteger("Capacity","Size",0);
        s_offset  = l_iniReader->GetInteger("Capacity","Offset",0);
        s_storageTimeout = l_iniReader->GetInteger("Basic","StorageTimeout",10000);
        delete l_iniReader;
        assert(s_readReqQSize!=0);
    }
    catch(const std::exception& e)    {
        std::cerr << e.what() << '\n';
        exit(-1);
    }
}
