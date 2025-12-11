#include "TraceGeneratorConfig.hpp"
#include "types.hpp"
#include "IniReader.hpp"
#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>

c_TraceGeneratorConfig::c_TraceGeneratorConfig(std::string p_TraceGeneratorConfigFile)    {
    try    {
        c_INIReader *l_iniReader = new c_INIReader(p_TraceGeneratorConfigFile);
        s_traceNum = l_iniReader->GetInteger("Trace","TraceCount",0);
        s_spatialInBlock = l_iniReader->GetInteger("Trace","SpatialInBlock",0);
        s_spatialAcrossBlock = l_iniReader->GetInteger("Trace","SpatialAcrossBlock",0);
        s_strideDistance = l_iniReader->GetInteger("Trace","StrideDistance",0);
        s_temporal = l_iniReader->GetInteger("Trace","Temporal",0);
        s_spatialInShuffler  = l_iniReader->GetInteger("Trace","SpatialInShuffler",0);
        s_spatialAcrossShuffler  = l_iniReader->GetInteger("Trace","SpatialAcrossShuffler",0);
        delete l_iniReader;
        assert(s_traceNum!=0);
    }
    catch(const std::exception& e)    {
        std::cerr << e.what() << '\n';
        exit(-1);
    }
}