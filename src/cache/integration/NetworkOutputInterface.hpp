#ifndef NETWORKOUTPUTINTERFACE_HPP
#define NETWORKOUTPUTINTERFACE_HPP
#include "SlavePort.hpp"
#include "Controller.hpp"
#include "types.hpp"

class c_ResponseController;
class c_TopologyConfig;
class c_Topology;
class c_Queue;

class c_NetworkOutputInterface : public c_Controller   {
private:
    c_Queue** m_coreInputQ[E_NUM_SENDQ_TYPE];
    c_Queue** m_memInputQ[E_NUM_SENDQ_TYPE];
    c_Queue*** m_switchOutputQPE;
    c_Queue*** m_switchOutputQMM;

    c_MasterPort** m_coreInputPorts[E_NUM_SENDQ_TYPE];
    c_MasterPort** m_memInputPorts[E_NUM_SENDQ_TYPE];
    c_SlavePort*** m_switchOutputPortsPE;
    c_SlavePort*** m_switchOutputPortsMM;

    t_Time m_clockCycle;
    c_TopologyConfig *m_topologyConfig;
    c_Topology *m_topology;
    bool **m_switchOutputIssuedThisCycle;
    t_Size m_numCores;
    t_Size m_numSlices;
    t_Size m_numMemChannel;
public:
    c_NetworkOutputInterface(std::string p_name, c_TopologyConfig *p_topologyConfig, c_Topology *p_topology);
    ~c_NetworkOutputInterface();

    c_MasterPort* f_getResponsePort(t_ResponseType p_responseType,t_PortNo p_portNo);
	c_SlavePort* f_getRequestPort(t_RequestType p_requestType,t_PortNo p_portNo);
	
    t_Time f_currentCycle();
    void f_clockCycle();
    void f_resetClock();
    void f_fetch();
	void f_decode();
    void f_execute();
	void f_issue();
    void f_retire();

    void f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction);
	c_Transaction* f_retireTransaction(t_PortNo p_portNo,t_ResponseType p_respType);
    
    std::string f_pipeString();
    bool f_getModuleDone();
    bool f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType);
    void f_watchDogTimer();
    t_NetworkChannel f_mapRespTypeToChannel(t_ResponseType p_respType);
    t_NetworkChannel f_mapReqTypeToChannel(t_RequestType p_requestType);
    void f_pullFromNoC(c_Queue *p_nOCQueue, c_Queue *p_nOIQueue);

    void f_bindPorts(c_MasterPort *p_masterPort, c_SlavePort *p_slavePort);
    void f_connect();
    void f_portCheck();
};

#endif
