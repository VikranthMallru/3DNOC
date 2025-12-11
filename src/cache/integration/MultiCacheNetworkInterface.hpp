#ifndef MULTICACHENETWORKINTERFACE_HPP
#define MULTICACHENETWORKINTERFACE_HPP
#include "SlavePort.hpp"
#include "MasterPort.hpp"
#include "Controller.hpp"
#include "types.hpp"
#include "CoreConfig.hpp"
#include "TypeToString.hpp"
#include "Queue.hpp"

class c_SimConfig;

class c_MultiCacheNetworkInterface  : public c_Controller {
private:
    t_CoreID m_coreId;
    t_Time m_clockCycle;
    c_CoreConfig *m_coreConfig;
    // Output Queues
	c_Queue** m_sendQ[t_ResponseType::E_NUM_SENDQ_TYPE];
    c_Queue** m_recvQ[t_RequestType::E_NUM_RECVQ_TYPE];
	// Output Port
	c_MasterPort **m_sendPorts[t_ResponseType::E_NUM_SENDQ_TYPE];
	t_Size m_numSendPort[t_ResponseType::E_NUM_SENDQ_TYPE];
    c_SlavePort **m_recvPorts[t_RequestType::E_NUM_RECVQ_TYPE];
	t_Size m_numRecvPort[t_RequestType::E_NUM_RECVQ_TYPE];
    t_Size m_sendQSize[t_ResponseType::E_NUM_SENDQ_TYPE];
    t_Size m_recvQSize[t_RequestType::E_NUM_RECVQ_TYPE];
    t_PortNo m_lastPort[t_RequestType::E_NUM_RECVQ_TYPE];
public:
	c_MultiCacheNetworkInterface(std::string p_moduleName, t_CoreID p_coreId, c_SimConfig *p_simConfig, c_CoreConfig *p_coreConfig);
    ~c_MultiCacheNetworkInterface();
    void f_clockCycle();
	t_Time f_currentCycle();
	void f_resetClock();
	void f_fetch();
	void f_decode();
	void f_issue();
	void f_execute();
	void f_retire();
    std::string f_pipeString();

    void f_fetchTransaction(t_PortNo p_portNo,c_Transaction *p_transaction);
    c_Transaction *f_retireTransaction(t_PortNo p_portNo, t_ResponseType p_respType);
	c_SlavePort* f_getRequestPort(t_RequestType p_requestType,t_PortNo p_portNo);
	c_MasterPort* f_getResponsePort(t_ResponseType p_responseType,t_PortNo p_portNo);
	void f_portCheck();
    bool f_getModuleDone();
    bool f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType);
    void f_watchDogTimer();
};

#endif
