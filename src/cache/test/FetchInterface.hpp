#include <cstring>
#include "Transaction.hpp"
#include "Instruction.hpp"
#include "MasterPort.hpp"
#include <vector>
#include <iostream>
#include "Controller.hpp"
#include "CacheLogger.hpp"
#include <string>
#include <sstream>
#include "TrafficGenerator.hpp"

#ifndef FETCHINTERFACE_H
#define FETCHINTERFACE_H

class c_CoreConfig;
class c_TraceReader;
class c_SimConfig;
class c_MemoryHierarchy;

class c_FetchInterface  : public c_Controller, public c_CacheLogger {
private:
    c_MemoryHierarchy *m_mh;

    c_MasterPort* m_readReqPort;
    c_MasterPort* m_writeReqPort;
    c_SlavePort* m_probeReqPort;
    c_SlavePort* m_readRespPort;
    c_MasterPort* m_writeDataPort;
    
    t_Time m_clockCycle;
    t_Time m_waitTime;
    
    vector<c_Transaction*> m_fetchRequestQueue;
    vector<t_VirtualAddress> m_fetchResponseQueue;
    vector<c_Transaction*> m_fetchOutstandingQueue;

    c_CoreConfig *m_coreConfig;
    c_SimConfig *m_simConfig;

    t_Size m_instFetched;
    t_Size m_programCounter;
    c_TraceReader *m_core;
    c_TrafficGenerator *m_trafficGenerator;
public:
    c_FetchInterface(c_MemoryHierarchy *p_mh,c_SimConfig *p_simConfig, c_CoreConfig *p_coreConfig);
    ~c_FetchInterface();
    c_MasterPort* f_getReadReqPort();
    c_MasterPort* f_getWriteReqPort();
    c_MasterPort* f_getWriteDataPort();

    c_SlavePort* f_getReadRespPort();
    c_SlavePort* f_getProbeReqPort();
    
    t_Time f_currentCycle();
    void f_resetClock();

    void f_clockCycle();
	void f_fetch();
	void f_decode();
    void f_execute();
	void f_issue();
    void f_retire();

    bool f_fetchResponse(t_VirtualAddress p_programCounter);
    bool f_fetchRequest(t_VirtualAddress p_programCounter);

    void f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction);
	c_Transaction* f_retireTransaction(t_PortNo p_portNo, t_ResponseType p_respType);
    std::string f_toString(c_Controller *p_ctrl,std::string p_message);
    bool f_getModuleDone();
    bool f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_reqType);
    void f_watchDogTimer();
    c_CoreConfig* f_getCoreConfig();

    void f_setCore(c_TraceReader *p_traceReader);
    void f_portCheck();
};

#endif
