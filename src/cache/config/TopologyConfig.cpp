#include "TopologyConfig.hpp"
#include "SimConfig.hpp"
#include "Topology.hpp"
#include "Coordinate.hpp"
#include "Repeater.hpp"
#include "Switch.hpp"
#include <fstream>
#include <assert.h>

c_TopologyConfig::c_TopologyConfig(c_SimConfig *p_simConfig, c_Topology *p_topology) {
    std::ifstream l_stream;
    s_pipeViewFile = p_simConfig->s_logPath + "/noc";
    s_numChannels = p_simConfig->s_numNoCChannels;
    s_numNodes = p_simConfig->s_numNodePerChannel;
    s_numRepeaters = p_simConfig->s_numRepeatersPerChannel;
    t_NodeId l_nodeIdCounter = 0;
    s_inputBufferSize = new t_Credit[s_numChannels];
    s_outputBufferSize = new t_Credit[s_numChannels];
    for(t_NetworkChannel l_channel = 0; l_channel < s_numChannels; l_channel++)    {
        l_stream.open(p_simConfig->s_nocConfigFile);
        l_stream >> s_inputBufferSize[l_channel];
        l_stream >> s_outputBufferSize[l_channel];
        for(t_NodeId l_tempId = 0; l_tempId < s_numNodes[l_channel] + s_numRepeaters[l_channel] ; l_tempId++)   {
            char l_nodeType;
            t_NodeId l_nodeId, l_nodeIdW, l_nodeIdZ, l_nodeIdY, l_nodeIdX;
            t_NodeId *l_nodeIdNeighbours;
            t_Credit *l_bufferSize;

            l_stream >> l_nodeType;
            l_stream >> l_nodeId;
            l_stream >> l_nodeIdW;
            l_stream >> l_nodeIdZ;
            l_stream >> l_nodeIdY;
            l_stream >> l_nodeIdX;
            c_Coordinate *l_coordinate = new c_Coordinate(l_channel,l_nodeIdW,l_nodeIdZ,l_nodeIdY,l_nodeIdX);
            c_Node *l_node = NULL;
            if(l_nodeType == 'R') {
                l_nodeIdNeighbours = new t_NodeId[t_RepeaterDirection::E_MAX_DIR_REPEATER];
                l_bufferSize = new t_Credit[t_RepeaterDirection::E_MAX_DIR_REPEATER];
                for (unsigned l_direction = t_RepeaterDirection::E_PREV; l_direction < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_direction) {
                    l_stream >> l_nodeIdNeighbours[l_direction];
                    l_stream >> l_bufferSize[l_direction];
                }
                l_node = new c_Repeater(p_topology, l_nodeId, l_coordinate, l_nodeIdNeighbours, l_bufferSize);
            }
            else if(l_nodeType == 'S') {
                l_nodeIdNeighbours = new t_NodeId[t_SwitchDirection::E_MAX_DIR_SWITCH];
                l_bufferSize = new t_Credit[t_SwitchDirection::E_MAX_DIR_SWITCH];
                for (unsigned l_direction = t_SwitchDirection::E_PE; l_direction < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_direction) {
                    l_stream >> l_nodeIdNeighbours[l_direction];
                    l_stream >> l_bufferSize[l_direction];
                }
                l_node = new c_Switch(p_topology, l_nodeId, l_coordinate, l_nodeIdNeighbours, l_bufferSize);
            }
            p_topology->f_addNode(l_node, l_channel);
        }
        if(l_channel !=  s_numChannels - 1) {
            l_stream.close();
        }
    }        
}

c_TopologyConfig::~c_TopologyConfig() {
    delete[] s_inputBufferSize;
    delete[] s_outputBufferSize;
}
