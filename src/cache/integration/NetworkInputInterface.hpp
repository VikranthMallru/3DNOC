#ifndef NETWORKINPUTINTERFACE_HPP
#define NETWORKINPUTINTERFACE_HPP
#include "SlavePort.hpp"
#include "Controller.hpp"
#include "types.hpp"

class c_RequestController;
class c_TopologyConfig;
class c_PrefetchController;
class c_Topology;
class c_Queue;

class c_NetworkInputInterface  : public c_Controller {
private:
    c_Queue** m_coreOutputQ[t_RequestType::E_NUM_RECVQ_TYPE];
    c_Queue** m_memOutputQ[t_RequestType::E_NUM_RECVQ_TYPE];
    c_Queue*** m_switchInputQPE;
    c_Queue*** m_switchInputQMM;

    c_SlavePort** m_coreOutputPorts[t_RequestType::E_NUM_RECVQ_TYPE];
    c_SlavePort** m_memOutputPorts[t_RequestType::E_NUM_RECVQ_TYPE];
    c_MasterPort*** m_switchInputPortsPE;
    c_MasterPort*** m_switchInputPortsMM;

    t_Time m_clockCycle;
    c_TopologyConfig *m_topologyConfig;
    c_Topology *m_topology;
    bool **m_switchInputIssuedThisCycle;
    t_Size m_numCores;
    t_Size m_numSlices;
    t_Size m_numMemChannel;
    t_SwitchDirection m_lastDir;
    c_PrefetchController *m_nocPrefetcher;
public:
    c_NetworkInputInterface(std::string p_name, c_TopologyConfig *p_topologyConfig, c_Topology *p_topology);
    ~c_NetworkInputInterface();

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

    t_NetworkChannel f_mapReqTypeToChannel(t_RequestType p_reqType);
    t_NetworkChannel f_mapRespTypeToChannel(t_ResponseType p_respType);
    void f_pushToNoC(c_Queue *p_nOCQueue, c_Queue *p_nIIQueue);

    void f_bindPorts(c_MasterPort *p_masterPort, c_SlavePort *p_slavePort);
    void f_connect();
    void f_portCheck();
};

#endif
