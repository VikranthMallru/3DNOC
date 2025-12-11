#ifndef TOPOLOGY_HPP
#define TOPOLOGY_HPP

#include "types.hpp"
#include "Controller.hpp"
#include "PipeView.hpp"
#include <vector>
#include <string>

class c_TopologyConfig;
class c_Node;
class c_NetworkInputInterface;
class c_NetworkOutputInterface;
class c_Routing;
class c_NetworkChannel;
class c_Switch;
class c_SimConfig;
class c_Transaction;

class c_Topology : public c_Controller, c_PipeView  {
private:
    t_Time m_clockCycle;
    c_SimConfig *m_simConfig;
    c_NetworkChannel **m_channels;
    c_NetworkInputInterface *m_inputInterface;
    c_NetworkOutputInterface *m_outputInterface;
    c_Routing *m_routingTable;
    c_TopologyConfig *m_config;
    t_NetworkChannel m_numChannels;    
    t_Size m_numActiveMem;
    t_CoreID* m_ActiveMemIds;
public:
    c_Node** f_getNetworkGraph(t_NetworkChannel p_channel);
    c_Switch* f_getSwitch(t_NetworkChannel p_channel, t_NodeId p_node);
    c_Topology(c_SimConfig *p_simConfig);
    ~c_Topology();
    c_TopologyConfig *f_getConfig();
    c_SimConfig *f_getSimConfig();
    c_NetworkInputInterface *f_getInputInterface();
    c_NetworkOutputInterface *f_getOutputInterface();
    t_NodeId f_getNumNodes(t_NetworkChannel p_channel);
    void f_addNode(c_Node *p_node, t_NetworkChannel p_channelId);
    void f_resetClock();
    void f_connect();
    void f_portCheck();
    c_Routing* f_getRoutingTable();
    t_NetworkChannel f_getNumChannels();
    void f_printChannel();

    void f_clockCycle();
	t_Time f_currentCycle();
	void f_fetch() {}
	void f_decode() {}
	void f_issue() {}
	void f_execute() {}
	void f_retire() {}

	c_Transaction* f_retireTransaction(t_PortNo p_portNo,  t_ResponseType p_respType) { return NULL; }
	virtual void f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction) {}
	bool f_getModuleDone();
	bool f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType) {}
	void f_watchDogTimer();
    t_NodeId f_findDestination(c_Transaction *p_transaction, t_SwitchDirection p_sourceDirection, t_NodeId p_destinationNode);
    std::string f_pipeString();
};
#endif
