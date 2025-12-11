#ifndef NETWORKCHANNEL_HPP
#define NETWORKCHANNEL_HPP

#include "types.hpp"
#include <vector>
#include <string>

class c_TopologyConfig;
class c_Node;
class c_Routing;

class c_NetworkChannel    {
private:
    c_Topology *m_topology;
    c_Node **m_nodes;
    t_NetworkChannel m_channelId;
public:
    c_Node** f_getNetworkGraph();
    c_NetworkChannel(c_SimConfig *p_simConfig, t_NetworkChannel p_channelId, c_Topology *p_topology);
    ~c_NetworkChannel();
    c_TopologyConfig *f_getConfig();
    t_NodeId f_getNumNodes();
    void f_addNode(c_Node *p_node);
    void f_resetClock();
    void f_connect();
    void f_portCheck();
    void f_clockCycle();
    t_NetworkChannel f_getChannelId();
    c_Node* f_getNode(t_NodeId p_nodeId);
    void f_watchDogTimer();
    std::string f_pipeString();
    void f_printNetwork();
    bool f_getModuleDone();
};
#endif