#include <string>
#include "types.hpp"

#ifndef TRACEGENCONFIG_H
#define TRACEGENCONFIG_H
class c_TraceGeneratorConfig	{
public:
    t_TraceLength s_traceNum;
    t_TraceLocality s_spatialInBlock;
    t_TraceLocality s_spatialAcrossBlock;
    t_TraceLocality s_temporal;
    t_TraceShuffler s_spatialInShuffler;
    t_TraceShuffler s_spatialAcrossShuffler;
    t_TraceStride s_strideDistance;
    c_TraceGeneratorConfig(std::string p_TraceGeneratorConfigFile);
};

#endif