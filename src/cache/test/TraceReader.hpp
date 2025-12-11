#include <string>
#include <fstream>
#include <queue>
#include "types.hpp"
#include "CacheLogger.hpp"
#include "Controller.hpp"

#ifndef TRACEREADER_H
#define TRACEREADER_H

class c_Instruction;
class c_LoadStoreInterface;
class c_FetchInterface;
class c_MemoryHierarchy;
class c_FixedLatencyStorage;
class c_SimConfig;

class c_TraceReader : public c_Controller, public c_CacheLogger {
private:
    t_CoreID m_coreId;
    std::queue<c_Instruction*> *m_instructionBuffer;
    c_LoadStoreInterface *m_lsu;
    c_FetchInterface *m_fu;
    c_FixedLatencyStorage *m_fls;
    c_MemoryHierarchy *m_mh;
    c_SimConfig* m_simSConfig;

    std::ifstream m_traceInput;
    std::string m_traceFile;
    bool m_traceDone;
    bool m_warmup;
    t_CacheLevel m_warmupLevel;

    t_Time m_clockCycle;

    t_Size m_instructionDecoded;
    t_Size m_instructionIssued;
    t_Size m_instructionRetired;
    c_SimConfig* m_simConfig;
public:
    c_TraceReader(c_SimConfig* p_simConfig,t_CoreID p_coreId, std::string p_name, std::string p_traceFile, 
        c_FetchInterface *p_fu, c_LoadStoreInterface *p_lsu, 
        c_MemoryHierarchy *p_mh, c_FixedLatencyStorage *p_fls, bool p_warmpUp, t_CacheLevel p_warmupLevel);
    ~c_TraceReader();
    t_Time f_currentCycle();
    void f_resetClock();
    void f_clockCycle();
    void f_fetch();
    void f_decode();
    void f_issue();
    void f_execute();
    void f_retire();
    c_Transaction* f_readFile(t_Size &p_nonLsCount);
    void f_warmUp();
    void f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction) {}
	c_Transaction* f_retireTransaction(t_PortNo p_portNo, t_ResponseType p_respType) {}
    std::string f_toString(c_Controller *p_ctrl,std::string p_message) { return p_message; }
    bool f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_reqType) {
        return true;
    }
    void f_displayStats();
    bool f_canFetch();
    void f_watchDogTimer() {}
    bool f_getModuleDone();
    void f_traceDone();
    void f_readTrace();
    void f_setInstructionRegister(t_Counter m_programCounter);
};

#endif