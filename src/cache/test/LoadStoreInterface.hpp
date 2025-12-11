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

#ifndef LOADSTOREINTERFACE_H
#define LOADSTOREINTERFACE_H

class c_CoreConfig;

class c_LoadStoreInterface  : public c_Controller, public c_CacheLogger {
private:
    c_MasterPort* m_readReqPort;
    c_MasterPort* m_writeReqPort;
    c_SlavePort* m_probeReqPort;
    c_SlavePort* m_readRespPort;
    c_MasterPort* m_writeDataPort;
    
    t_Time m_clockCycle;
    
    vector<c_Transaction*> m_rob;
    vector<c_Transaction*> m_loadQueue;
    vector<c_Transaction*> m_storeQueue;
    vector<c_Transaction*> m_storeDataQueue;

    c_CoreConfig *m_coreConfig;

    t_Size m_loadsExecuted;
    t_Size m_storesExecuted;
public:
    c_LoadStoreInterface(c_CoreConfig *p_coreConfig);
    ~c_LoadStoreInterface();
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

    void f_portCheck();

    bool f_pushTransaction(c_Transaction *p_transaction);

    void f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction);
	c_Transaction* f_retireTransaction(t_PortNo p_portNo, t_ResponseType p_respType);
    std::string f_toString(c_Controller *p_ctrl,std::string p_message);
    bool f_getModuleDone();
    bool f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_reqType);
    void f_watchDogTimer();
};

#endif
