#include "types.hpp"
#include <string>

class c_Coordinate;
class c_Topology;
class c_SimConfig;

class c_TopologyConfig {
public:
    std::string s_pipeViewFile;
    t_NodeId *s_numNodes;
    t_NodeId *s_numRepeaters;
    t_NetworkChannel s_numChannels;

    t_RoutingAlgorithm s_routingAlgorithm;

    t_Credit *s_inputBufferSize;
    t_Credit *s_outputBufferSize;
    
    t_DeviceType f_getDeviceType(c_Coordinate *p_coordinate);
    t_NodeId f_getNodeId(c_Coordinate *p_coordinate);
    c_Coordinate* f_getCoordinate(t_NodeId p_nodeId);
    c_TopologyConfig(c_SimConfig *p_simConfig, c_Topology *p_topology);
    ~c_TopologyConfig();
};