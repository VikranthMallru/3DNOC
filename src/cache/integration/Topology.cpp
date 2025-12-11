#include "Topology.hpp"
#include "Node.hpp"
#include "Controller.hpp"
#include "Switch.hpp"
#include "Repeater.hpp"
#include "TopologyConfig.hpp"
#include "NetworkInputInterface.hpp"
#include "NetworkOutputInterface.hpp"
#include "Routing.hpp"
#include "Queue.hpp"
#include "NetworkChannel.hpp"
#include "assert.h"
#include "SimConfig.hpp"
#include "PipeView.hpp"

c_Topology::c_Topology(c_SimConfig *p_simConfig) : c_Controller("NOC") {
    m_clockCycle = 0;
    m_simConfig = p_simConfig;
    m_numChannels = p_simConfig->s_numNoCChannels;
    m_channels = new c_NetworkChannel*[m_numChannels];
    for(t_NetworkChannel l_channel = 0; l_channel < m_numChannels; l_channel++)    {
        m_channels[l_channel] = new c_NetworkChannel(p_simConfig, l_channel, this);
    }
    m_config = new c_TopologyConfig(p_simConfig, this);
    m_inputInterface = new c_NetworkInputInterface("NII",m_config, this);
    m_outputInterface = new c_NetworkOutputInterface("NOI",m_config, this);
    m_routingTable = new c_Routing(this, p_simConfig->s_routingAlgorithmPerChannel);
    m_routingTable->f_loadRoutingTable();
    initPipeView(m_config->s_pipeViewFile, p_simConfig->s_logConfig);
    for(t_NetworkChannel l_channel = 0; l_channel < m_numChannels; l_channel++)    {
        m_channels[l_channel]->f_printNetwork();
    }    
    m_numActiveMem = 0;
    for(t_CoreID l_coreID = 0; l_coreID < m_simConfig->s_numMemoryChannels; l_coreID++){
        if(m_simConfig->s_activeMem[l_coreID]) m_numActiveMem++;
    }
    m_ActiveMemIds = new t_CoreID[m_numActiveMem];
    for(t_CoreID l_coreID = 0, l_activeCoreId = 0; l_coreID < m_simConfig->s_numMemoryChannels; l_coreID++){
        if(m_simConfig->s_activeMem[l_coreID]) m_ActiveMemIds[l_activeCoreId] = l_coreID, l_activeCoreId++;
    }
}

void c_Topology::f_printChannel()   {
    for(t_NetworkChannel l_channel = 0; l_channel < m_numChannels; l_channel++)    {
        std::cout<<l_channel<<"\t"<<m_channels[l_channel]->f_getChannelId()<<std::endl;
    }
}

c_Topology::~c_Topology()    {
    delete m_config;
    delete m_inputInterface;
    delete m_outputInterface;
    delete m_routingTable;
    for(t_NetworkChannel l_channel = 0; l_channel < m_config->s_numChannels; l_channel++)    {
        delete m_channels[l_channel];
    }
    delete[] m_channels;
}

c_Node** c_Topology::f_getNetworkGraph(t_NetworkChannel p_channel)  {
    return m_channels[p_channel]->f_getNetworkGraph();
}

t_NodeId c_Topology::f_getNumNodes(t_NetworkChannel p_channel)    {
    return m_channels[p_channel]->f_getNumNodes();
}

c_TopologyConfig *c_Topology::f_getConfig() {
    return m_config;
}

c_NetworkInputInterface *c_Topology::f_getInputInterface()   {
    return m_inputInterface;
}

c_NetworkOutputInterface *c_Topology::f_getOutputInterface()   {
    return m_outputInterface;
}

void c_Topology::f_addNode(c_Node *p_node, t_NetworkChannel p_channelId)  {
    m_channels[p_channelId]->f_addNode(p_node);
}

void c_Topology::f_resetClock() {
    m_clockCycle = 0;
    for(t_NetworkChannel l_channel = 0; l_channel < m_config->s_numChannels; l_channel++)    {
        m_channels[l_channel]->f_resetClock();
    }
    m_inputInterface->f_resetClock();
    m_outputInterface->f_resetClock();
}

void c_Topology::f_connect()   {
    for(t_NetworkChannel l_channel = 0; l_channel < m_config->s_numChannels; l_channel++)    {
        m_channels[l_channel]->f_connect();
    }
    m_inputInterface->f_connect();
    m_outputInterface->f_connect();
}

void c_Topology::f_portCheck()  {
    m_inputInterface->f_portCheck();
    m_outputInterface->f_portCheck();
    for(t_NetworkChannel l_channel = 0; l_channel < m_config->s_numChannels; l_channel++)    {
        m_channels[l_channel]->f_portCheck();
    }
}

c_Routing* c_Topology::f_getRoutingTable()  {
    return m_routingTable;
}

c_Switch* c_Topology::f_getSwitch(t_NetworkChannel p_channel, t_NodeId p_node)  {
    return (c_Switch*) m_channels[p_channel]->f_getNode(p_node);
}


void c_Topology::f_clockCycle() {
    m_clockCycle++;
    std::string l_pipe = f_pipeString();
	f_logPipe(this,l_pipe);
    m_outputInterface->f_clockCycle();
	for(t_NetworkChannel l_channel = 0; l_channel < m_config->s_numChannels; l_channel++)    {
        m_channels[l_channel]->f_clockCycle();
    }
    m_inputInterface->f_clockCycle();
}

t_Time c_Topology::f_currentCycle() {
    return m_clockCycle;
}

bool c_Topology::f_getModuleDone()  {
    bool l_done = true;
    for(t_NetworkChannel l_channel = 0; l_channel < m_config->s_numChannels; l_channel++)    {
        l_done &= m_channels[l_channel]->f_getModuleDone();
    }
    l_done &= m_inputInterface->f_getModuleDone();
    l_done &= m_outputInterface->f_getModuleDone();
    return l_done;
}

std::string c_Topology::f_pipeString()  {
    std::string l_pipeString = "Cycle-" + std::to_string(m_clockCycle) + ":\n";
    for(t_NetworkChannel l_channel = 0; l_channel < m_config->s_numChannels; l_channel++)    {
        l_pipeString = l_pipeString + m_channels[l_channel]->f_pipeString();
    }
    l_pipeString = l_pipeString + m_inputInterface->f_pipeString();
    l_pipeString = l_pipeString + m_outputInterface->f_pipeString();
    return l_pipeString;
}

void c_Topology::f_watchDogTimer()  {
    for(t_NetworkChannel l_channel = 0; l_channel < m_config->s_numChannels; l_channel++)    {
        m_channels[l_channel]->f_watchDogTimer();
    }
    m_inputInterface->f_watchDogTimer();
    m_outputInterface->f_watchDogTimer();
}

t_NetworkChannel c_Topology::f_getNumChannels()    {
    return m_numChannels;
}

t_NodeId c_Topology::f_findDestination(c_Transaction *p_transaction, t_SwitchDirection p_sourceDirection, t_NodeId p_destination)   {
    t_RequestType l_reqType = p_transaction->f_getRequestType();
    t_PhysicalAddress l_address =  p_transaction->f_getPhysicalAddress() >> 6;
    t_NodeId l_destination = 0;
    switch(l_reqType)   {
    case t_RequestType::E_CORE_READ:
    case t_RequestType::E_CORE_WRITE:
    case t_RequestType::E_PROBE_RESP:
        if(p_sourceDirection == t_SwitchDirection::E_PE)   {
                l_destination = l_address % m_numActiveMem;
            return m_ActiveMemIds[l_destination];
        }
        else {
            p_transaction->f_logError(this,"Invalid Source and Request Type Pair");
        }
    case t_RequestType::E_PROBE_REQ:
        if(p_sourceDirection == t_SwitchDirection::E_MM)   {
                l_destination = p_transaction->f_getRequestingCore();
            return l_destination;
        }
        else {
            p_transaction->f_logError(this,"Invalid Source and Request Type Pair");
        }
    case t_RequestType::E_FILL:
        if(p_sourceDirection == t_SwitchDirection::E_MM)   {
                l_destination = p_transaction->f_getRequestingCore();
            return l_destination;
        }
        else {
            p_transaction->f_logError(this,"Invalid Source and Request Type Pair");
        }
    default:
        p_transaction->f_logError(this,"Invalid Request Type");
    }
}

c_SimConfig *c_Topology::f_getSimConfig()   {
    return m_simConfig;
}
