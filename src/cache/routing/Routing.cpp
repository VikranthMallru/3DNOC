#include "Routing.hpp"
#include "Topology.hpp"
#include "Node.hpp"
#include "NetworkPacket.hpp"
#include "NetworkChannel.hpp"
#include "TopologyConfig.hpp"
#include <vector>

using namespace std;

c_Routing::c_Routing(c_Topology *p_topology,t_RoutingAlgorithm *p_routingAlgorithm) {
    m_topology = p_topology;
    m_routingAlgorithm = p_routingAlgorithm;
    m_routingTable = new t_Size**[m_topology->f_getConfig()->s_numChannels];
    
    m_distance = new t_Counter**[m_topology->f_getConfig()->s_numChannels];
    for(t_NetworkChannel l_channel = 0; l_channel < m_topology->f_getConfig()->s_numChannels; l_channel++)    {
        t_NodeId l_numNodes = m_topology->f_getNumNodes(l_channel);
        m_routingTable[l_channel] = new t_Size*[l_numNodes];
        for(t_NodeId l_nodeSource = 0; l_nodeSource < l_numNodes; l_nodeSource++)    {
            m_routingTable[l_channel][l_nodeSource] = new t_Size[l_numNodes];
            for(t_NodeId l_nodeDest = 0; l_nodeDest < l_numNodes; l_nodeDest++)    {
                m_routingTable[l_channel][l_nodeSource][l_nodeDest] = INT_MAX;
            }
        }
        m_distance[l_channel] = new t_Counter*[l_numNodes];
        for(t_Counter l_source = 0; l_source < l_numNodes; l_source++)	{
            m_distance[l_channel][l_source] = new t_Counter[l_numNodes];
            for(t_Counter l_destination = 0; l_destination < l_numNodes; l_destination++)	{
                if(l_source == l_destination)   {
                    m_distance[l_channel][l_source][l_destination] = 0;
                }
                else    {
                    m_distance[l_channel][l_source][l_destination] = M_MAX_NODE;
                }
            }
        }
        f_floydWarshall(l_channel,l_numNodes);
    }
}

void c_Routing::f_floydWarshall(t_NetworkChannel l_channel, t_NodeId p_numNodes)   {
    for (t_Counter l_count = 0; l_count < p_numNodes; l_count++)    {
        for (t_Counter l_source = 0; l_source < p_numNodes; l_source++)   {
            for (t_Counter l_destination = 0; l_destination < p_numNodes; l_destination++) {
                if (m_distance[l_channel][l_source][l_destination] > m_distance[l_channel][l_source][l_count] + m_distance[l_channel][l_count][l_destination])    {
                    m_distance[l_channel][l_source][l_destination] = m_distance[l_channel][l_source][l_count] + m_distance[l_channel][l_count][l_destination];
                }
            }
        }
    }
}

c_Routing::~c_Routing() {
    for(t_NetworkChannel l_channel = 0; l_channel < m_topology->f_getConfig()->s_numChannels; l_channel++)    {
        t_NodeId l_numNodes = m_topology->f_getNumNodes(l_channel);
        for(t_NodeId l_nodeSource = 0; l_nodeSource < l_numNodes; l_nodeSource++)    {
            delete[] m_routingTable[l_channel][l_nodeSource];
        }
        delete[] m_routingTable[l_channel];
        for(t_Counter l_source = 0; l_source < l_numNodes; l_source++)	{
            delete m_distance[l_channel][l_source];
        }
        delete[] m_distance[l_channel];
    }
    delete[] m_distance;
    delete[] m_routingTable; 
}

void c_Routing::f_updateRoutingTable()    {
}

void c_Routing::f_loadRoutingTable()  {
    for(t_NetworkChannel l_channel = 0; l_channel < m_topology->f_getConfig()->s_numChannels; l_channel++)  {
        t_NodeId l_numNodes = m_topology->f_getNumNodes(l_channel);
        c_Node **l_nodes = m_topology->f_getNetworkGraph(l_channel);
        for(t_Size l_sourceNodeId = 0; l_sourceNodeId < l_numNodes; l_sourceNodeId++)   {
            for(t_Size l_destNodeId = 0; l_destNodeId < l_numNodes; l_destNodeId++)   {
                f_XYZW_Routing(l_channel, l_nodes[l_sourceNodeId],l_nodes[l_destNodeId]);
            }            
        }
    }
}

void c_Routing::f_XYZW_Routing(t_NetworkChannel p_channel,c_Node *p_sourceNode, c_Node *p_destNode)  {
    c_Coordinate *l_srcNodeCord = p_sourceNode->f_getCoordinate();
    c_Coordinate *l_destNodeCord = p_destNode->f_getCoordinate();
    if(l_destNodeCord->f_getX() > l_srcNodeCord->f_getX())    {
        m_routingTable[p_channel][p_sourceNode->f_getNodeId()][p_destNode->f_getNodeId()] = E_EAST;
    }
    else if(l_destNodeCord->f_getX() < l_srcNodeCord->f_getX())    {
        m_routingTable[p_channel][p_sourceNode->f_getNodeId()][p_destNode->f_getNodeId()] = E_WEST;
    }
    else    {
        if(l_destNodeCord->f_getY() > l_srcNodeCord->f_getY())    {
            m_routingTable[p_channel][p_sourceNode->f_getNodeId()][p_destNode->f_getNodeId()] = E_NORTH;
        }
        else if(l_destNodeCord->f_getY() < l_srcNodeCord->f_getY())    {
            m_routingTable[p_channel][p_sourceNode->f_getNodeId()][p_destNode->f_getNodeId()] = E_SOUTH;
        }
        else    {
            if(l_destNodeCord->f_getZ() > l_srcNodeCord->f_getZ())    {
                m_routingTable[p_channel][p_sourceNode->f_getNodeId()][p_destNode->f_getNodeId()] = E_UP;
            }
            else if(l_destNodeCord->f_getZ() < l_srcNodeCord->f_getZ())    {
                m_routingTable[p_channel][p_sourceNode->f_getNodeId()][p_destNode->f_getNodeId()] = E_DOWN;
            }
        }    
    }
}

t_Size c_Routing::f_getDirection(c_NetworkPacket *p_packet) {
    return m_routingTable[p_packet->f_getChannel()][p_packet->f_getCurrentNode()][p_packet->f_getDestination()];
}

t_Counter*** c_Routing::f_getDistance() {
    return m_distance;
}

void c_Routing::f_printRoutingTable()   {
    for(t_NetworkChannel l_channel = 0; l_channel < m_topology->f_getConfig()->s_numChannels; l_channel++)  {
        t_NodeId l_numNodes = m_topology->f_getNumNodes(l_channel);
        c_Node **l_nodes = m_topology->f_getNetworkGraph(l_channel);
        for(t_Size l_sourceNodeId = 0; l_sourceNodeId < l_numNodes; l_sourceNodeId++)   {
            for(t_Size l_destNodeId = 0; l_destNodeId < l_numNodes; l_destNodeId++)   {
                std::cout<<m_routingTable[l_channel][l_sourceNodeId][l_destNodeId]<<"\t";
            }
        }
        std::cout<<std::endl;
    }
}