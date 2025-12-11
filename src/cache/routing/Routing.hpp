#ifndef ROUTING_H
#define ROUTING_H

#include "types.hpp"

class c_Topology;
class c_Node;
class c_NetworkPacket;
class c_TopologyConfig;

class c_Routing {
private:
    t_NodeId ***m_routingTable;
    c_Topology *m_topology;
    t_RoutingAlgorithm *m_routingAlgorithm;
    t_Counter*** m_distance;
    void f_floydWarshall(t_NetworkChannel l_channel, t_NodeId p_numNodes);
    void f_XYZW_Routing(t_NetworkChannel p_channel,c_Node *p_sourceNode, c_Node *p_destNode);
public:
    c_Routing(c_Topology *p_topology,t_RoutingAlgorithm *p_routingAlgorithm);
    ~c_Routing();
    t_Size f_getDirection(c_NetworkPacket *p_Packet);
    t_Counter*** f_getDistance();
    void f_updateRoutingTable();
    void f_loadRoutingTable();
    void f_printRoutingTable();
};
#endif