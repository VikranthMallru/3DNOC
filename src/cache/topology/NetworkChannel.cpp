#include "Topology.hpp"
#include "Node.hpp"
#include "Switch.hpp"
#include "Repeater.hpp"
#include "TopologyConfig.hpp"
#include "SimConfig.hpp"
#include "NetworkInputInterface.hpp"
#include "NetworkOutputInterface.hpp"
#include "Routing.hpp"
#include "Queue.hpp"
#include "NetworkChannel.hpp"

c_NetworkChannel::c_NetworkChannel(c_SimConfig *p_simConfig, t_NetworkChannel p_channelId, c_Topology *p_topology) {
    m_channelId = p_channelId;
    m_topology = p_topology;
    m_nodes = new c_Node*[p_simConfig->s_numNodePerChannel[m_channelId] + p_simConfig->s_numRepeatersPerChannel[m_channelId]];
}

c_NetworkChannel::~c_NetworkChannel()    {
    for(t_NodeId l_nodeId = 0; l_nodeId < f_getNumNodes(); l_nodeId++)   {
        delete m_nodes[l_nodeId];
    }
    delete[] m_nodes;
}

t_NetworkChannel c_NetworkChannel::f_getChannelId()  {
    return m_channelId;
}

c_Node** c_NetworkChannel::f_getNetworkGraph()  {
    return m_nodes;
}

t_NodeId c_NetworkChannel::f_getNumNodes()    {
    return m_topology->f_getConfig()->s_numNodes[m_channelId] + m_topology->f_getConfig()->s_numRepeaters[m_channelId];
}

c_TopologyConfig *c_NetworkChannel::f_getConfig() {
    return m_topology->f_getConfig();
}

void c_NetworkChannel::f_addNode(c_Node *p_node)  {
    m_nodes[p_node->f_getNodeId()] = p_node;
}

void c_NetworkChannel::f_resetClock() {
    for(t_NodeId l_nodeId = 0; l_nodeId < f_getNumNodes(); l_nodeId++)   {
        m_nodes[l_nodeId]->f_resetClock();
    }
}

void c_NetworkChannel::f_printNetwork() {
    std::cout<<"Channel: "<<m_channelId<<std::endl;
    for(t_NodeId l_nodeId = 0; l_nodeId < f_getNumNodes(); l_nodeId++)   {
        std::cout<<"Node: "<<l_nodeId <<":\t";
        t_NodeId *l_neighbourId = m_nodes[l_nodeId]->f_getNeighbour();
        if(m_nodes[l_nodeId]->f_getType() == E_SWITCH)  {
            for (unsigned l_dir = t_SwitchDirection::E_EAST; l_dir < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_dir) {
                std::cout<<l_neighbourId[l_dir]<<"\t";
            }
            std::cout<<std::endl;
        }
        else if(m_nodes[l_nodeId]->f_getType() == E_REPEATER)   {
            for (unsigned l_dir = t_RepeaterDirection::E_PREV; l_dir < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_dir) {
                std::cout<<l_neighbourId[l_dir]<<"\t";
            }
            std::cout<<std::endl;
        }
    }
}

void c_NetworkChannel::f_connect()   {
    for(t_NodeId l_nodeId = 0; l_nodeId < f_getNumNodes(); l_nodeId++)   {
        if(m_nodes[l_nodeId]->f_getType() == E_SWITCH)  {
            for (unsigned l_dir = t_SwitchDirection::E_EAST; 
                l_dir < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_dir) {
                unsigned l_oppDir;
                t_NodeId *l_neighbourId = m_nodes[l_nodeId]->f_getNeighbour();
                if(l_neighbourId[l_dir] != M_MAX_NODE)  {
                    if(m_nodes[l_neighbourId[l_dir]]->f_getType() == E_REPEATER)  {
                        l_oppDir = c_Switch::f_oppositeDirection(m_nodes[l_neighbourId[l_dir]], l_nodeId);
                    }
                    else    {
                        l_oppDir = c_Switch::f_oppositeDirection((t_SwitchDirection)l_dir);
                    }
                    m_nodes[l_nodeId]->f_getMasterPort(l_dir)->f_bind(m_nodes[l_neighbourId[l_dir]]->f_getSlavePort(l_oppDir));
                    m_nodes[l_nodeId]->f_getSlavePort(l_dir)->f_bind(m_nodes[l_neighbourId[l_dir]]->f_getMasterPort(l_oppDir));
                    m_nodes[l_neighbourId[l_dir]]->f_getMasterPort(l_oppDir)->f_bind(m_nodes[l_nodeId]->f_getSlavePort(l_dir));
                    m_nodes[l_neighbourId[l_dir]]->f_getSlavePort(l_oppDir)->f_bind(m_nodes[l_nodeId]->f_getMasterPort(l_dir));
                }
            }
        }
        else if(m_nodes[l_nodeId]->f_getType() == E_REPEATER)   {
            for (unsigned l_dir = t_RepeaterDirection::E_PREV; 
                l_dir < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_dir) {
                unsigned l_oppDir; 
                t_NodeId *l_neighbourId = m_nodes[l_nodeId]->f_getNeighbour();
                if(l_neighbourId[l_dir] != M_MAX_NODE)  {
                    if(m_nodes[l_neighbourId[l_dir]]->f_getType() == E_REPEATER)  {
                        l_oppDir = c_Repeater::f_oppositeDirection((t_RepeaterDirection) l_dir);
                    }
                    else    {
                        l_oppDir = c_Repeater::f_oppositeDirection(m_nodes[l_neighbourId[l_dir]], l_nodeId);
                    }
                    m_nodes[l_nodeId]->f_getMasterPort(l_dir)->f_bind(m_nodes[l_neighbourId[l_dir]]->f_getSlavePort(l_oppDir));
                    m_nodes[l_nodeId]->f_getSlavePort(l_dir)->f_bind(m_nodes[l_neighbourId[l_dir]]->f_getMasterPort(l_oppDir));
                    m_nodes[l_neighbourId[l_dir]]->f_getMasterPort(l_oppDir)->f_bind(m_nodes[l_nodeId]->f_getSlavePort(l_dir));
                    m_nodes[l_neighbourId[l_dir]]->f_getSlavePort(l_oppDir)->f_bind(m_nodes[l_nodeId]->f_getMasterPort(l_dir));
                }
            }
        }
    }
}

void c_NetworkChannel::f_clockCycle()   {
    for(t_NodeId l_nodeId = 0; l_nodeId < f_getNumNodes(); l_nodeId++)   {
        m_nodes[l_nodeId]->f_clockCycle();
    }
}

void c_NetworkChannel::f_portCheck()  {
    for(t_NodeId l_nodeId = 0; l_nodeId < f_getNumNodes(); l_nodeId++)   {
        m_nodes[l_nodeId]->f_portCheck();
    }
}

c_Node* c_NetworkChannel::f_getNode(t_NodeId p_nodeId) {
    return m_nodes[p_nodeId];
}

void c_NetworkChannel::f_watchDogTimer() {
    for(t_NodeId l_nodeId = 0; l_nodeId < f_getNumNodes(); l_nodeId++)   {
        m_nodes[l_nodeId]->f_watchDogTimer();
    }
}

std::string c_NetworkChannel::f_pipeString()   {
    std::string l_pipeString = "Channel[" + std::to_string(m_channelId) + "]:\n";
    for(t_NodeId l_nodeId = 0; l_nodeId < f_getNumNodes(); l_nodeId++)   {
        l_pipeString = l_pipeString + "{" + m_nodes[l_nodeId]->f_pipeString() + "}\n";
    }
    return l_pipeString;
}

bool c_NetworkChannel::f_getModuleDone()  {
    bool l_done = false;
    for(t_NodeId l_nodeId = 0; l_nodeId < f_getNumNodes(); l_nodeId++)   {
        l_done &= m_nodes[l_nodeId]->f_getModuleDone();
    }
    return l_done;
}