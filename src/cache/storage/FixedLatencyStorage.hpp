#include "SlavePort.hpp"
#include "types.hpp"
#include "StorageConfig.hpp"
#include "Controller.hpp"
#include "FixedLatencyStoragePacket.hpp"
#include <vector>
#include "SimConfig.hpp"

using namespace std;

class c_FixedLatencyStorage : public c_Controller, public c_CacheLogger  {
private:
    c_SlavePort* m_memRdReqPort;
    c_SlavePort* m_memWrReqPort;
    c_SlavePort* m_memWrDataPort;

    c_MasterPort* m_memRdRespPort;
    c_MasterPort* m_memPrbReqPort;

    t_Time m_clockCycle;
    t_Time m_fixedLatency;
    t_Data m_actualDataStore;
    t_Address m_size;
    unsigned m_offset;

    vector<c_FixedLatencyStoragePacket*> m_readQueue;
    vector<c_FixedLatencyStoragePacket*> m_writeQueue;
    vector<c_FixedLatencyStoragePacket*> m_probeQueue;

    t_Credit m_coreRdCredit;
    t_Credit m_coreWrCredit;
    c_StorageConfig *m_config;
    c_SimConfig *m_simConfig;
    t_Counter m_priorityRR; // Packets should be taken from which queue(read or write queue)
    t_CoreID m_coreID;
public:
    c_FixedLatencyStorage(std::string p_name, c_StorageConfig *p_storageConfig, t_CoreID p_CoreID, c_SimConfig* p_config);
    ~c_FixedLatencyStorage();

    c_MasterPort* f_getResponsePort(t_ResponseType p_responseType);
    c_SlavePort* f_getRequestPort(t_RequestType p_requestType);
    
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
    t_FixedLatencyRequestType convertToFixedLatencyRequestType(t_RequestType p_reqType);

    std::string f_toString(c_Controller *p_ctrl,std::string p_message);
    bool f_getModuleDone();
    bool f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType);
    void f_watchDogTimer();

    bool f_pushTransaction(c_Transaction *p_transaction);
    void f_portCheck();
};
