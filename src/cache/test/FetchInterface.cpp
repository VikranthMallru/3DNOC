#include "FetchInterface.hpp"
#include "CoreConfig.hpp"
#include "SlavePort.hpp"
#include "MasterPort.hpp"
#include "RequestPool.hpp"
#include "TraceReader.hpp"
#include "SimConfig.hpp"
#include "MemoryHierarchy.hpp"
#include <assert.h>
#include "TrafficGenerator.hpp"

c_FetchInterface::c_FetchInterface(c_MemoryHierarchy *p_mh,c_SimConfig *p_simConfig, c_CoreConfig *p_coreConfig) : 
        c_Controller(p_coreConfig->s_name)   {
    m_clockCycle = 0;
    m_mh = p_mh;
    m_coreConfig = p_coreConfig;
    m_simConfig = p_simConfig;

    m_readReqPort = new c_MasterPort(this,"coreFetchRdReqPort",p_coreConfig->s_coreId,t_ResponseType::E_MISS);
    m_writeReqPort = new c_MasterPort(this,"coreFetchWrReqPort",p_coreConfig->s_coreId,t_ResponseType::E_VICTIM);
    m_readRespPort = new c_SlavePort(this,"coreFetchRdRespPort",p_coreConfig->s_coreId,t_RequestType::E_FILL);
    m_writeDataPort = new c_MasterPort(this,"coreFetchWriteDataPort",p_coreConfig->s_coreId,t_ResponseType::E_PROBE_RESP_SEND);
    m_probeReqPort = new c_SlavePort(this,"coreFetchProbeReqPort",p_coreConfig->s_coreId,t_RequestType::E_PROBE_REQ);

    m_instFetched = 0;
    m_trafficGenerator = new c_TrafficGenerator(m_coreConfig->s_trafficMode, m_coreConfig->s_seed, m_coreConfig->s_param1, m_coreConfig->s_param2);
    m_waitTime = m_trafficGenerator->f_nextWait();
    m_programCounter = ((1 + m_coreConfig->s_coreId) << 56);  // Address Munging
}

void c_FetchInterface::f_setCore(c_TraceReader *p_traceReader)        {
    m_core = p_traceReader;
}

c_FetchInterface::~c_FetchInterface()   {
    delete m_readReqPort;
    delete m_writeReqPort;
    delete m_readRespPort;
    delete m_writeDataPort;
    delete m_probeReqPort;
}

t_Time c_FetchInterface::f_currentCycle() {
    return m_clockCycle;
}

void c_FetchInterface::f_resetClock()	{
	m_clockCycle = 0;
}

void c_FetchInterface::f_clockCycle()    {
    f_watchDogTimer();
    f_retire();
    f_execute();
    f_issue();
    f_decode();
    f_fetch();
    m_clockCycle++;
}

c_MasterPort* c_FetchInterface::f_getReadReqPort()   {
    return m_readReqPort;
}

c_MasterPort* c_FetchInterface::f_getWriteReqPort()  {
    return m_writeReqPort;
}

c_SlavePort* c_FetchInterface::f_getReadRespPort()   {
    return m_readRespPort;
}

c_SlavePort* c_FetchInterface::f_getProbeReqPort()  {
    return m_probeReqPort;
}

c_MasterPort* c_FetchInterface::f_getWriteDataPort()   {
    return m_writeDataPort;
}

c_Transaction* c_FetchInterface::f_retireTransaction(t_Credit p_portNo, t_ResponseType p_respType) {
    c_Transaction* l_transaction = NULL;
    if(p_respType == t_ResponseType::E_MISS)    {
        if(!m_fetchRequestQueue.empty())    {
            l_transaction = m_fetchRequestQueue.front();
            m_fetchRequestQueue.erase(m_fetchRequestQueue.begin());
            m_fetchOutstandingQueue.push_back(l_transaction);
            l_transaction->f_logDebug(this,"Sending fetch request to memory");
        }
    }
    return l_transaction;
}

void c_FetchInterface::f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction)  {
    auto l_indexToRemove = m_fetchOutstandingQueue.end();
    for(auto l_index = m_fetchOutstandingQueue.begin(); l_index != m_fetchOutstandingQueue.end(); l_index++) {
        if(*l_index == p_transaction)    {
            if(p_transaction->f_getRequestType() == t_RequestType::E_FILL)    {
                p_transaction->f_logDebug(this,"Fetch Response received from memory");
                l_indexToRemove = l_index;
                m_core->f_decode();
                m_instFetched++;
                c_RequestPool<c_Transaction>::f_deAllocate(p_transaction);
                if(m_instFetched == m_simConfig->m_maxInstFetched)  {
                    m_mh->f_finishSimulation();
                }
                else    {
                    //std::cout<<"Core-"<<m_coreConfig->s_coreId<<" simulates "<<m_instFetched<<" instructions\n";
                }
                break;
            }
            else    {
                p_transaction->f_logError(this,"Unknown transaction type");
            }
        }
    }
    if(l_indexToRemove != m_fetchOutstandingQueue.end())  {
        m_fetchOutstandingQueue.erase(l_indexToRemove);
    }
    else    {
        p_transaction->f_logError(this,"Wrong destination: Should be in other core or LSU");
    }
}

void c_FetchInterface::f_fetch()  {
    m_readRespPort->f_clockCycle();
}

void c_FetchInterface::f_decode() {}

void c_FetchInterface::f_execute()    {
    if(m_fetchRequestQueue.size() < m_coreConfig->s_fetchQSize && m_core->f_canFetch()
        && m_simConfig->s_activeCores[m_coreConfig->s_coreId] && m_waitTime == 0)  {
        c_Transaction *l_transaction = c_RequestPool<c_Transaction>::f_allocate(m_clockCycle);
        t_Data l_data = new unsigned char[64];
        l_transaction->f_initTransaction(m_simConfig->s_numCores,m_programCounter,t_RequestType::E_CORE_READ,l_data,t_ResponseType::E_NUM_SENDQ_TYPE,t_ProbeType::E_PRB_INVALIDATE,0,0, t_CacheState::E_EXCLUSIVE, m_clockCycle,m_coreConfig->s_coreId);;
        l_transaction->f_setLineType(t_LineType::E_INSTRUCTION);
        l_transaction->f_logDebug(this,"Creating new transaction");
        m_fetchRequestQueue.push_back(l_transaction);
        m_programCounter = m_programCounter + (1 << 6);
        m_waitTime = m_trafficGenerator->f_nextWait();
    }
    else m_waitTime--;
}

c_CoreConfig* c_FetchInterface::f_getCoreConfig() {
    return m_coreConfig;
}

void c_FetchInterface::f_issue()  {}

void c_FetchInterface::f_retire()  {
    m_readReqPort->f_clockCycle();
}

bool c_FetchInterface::f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_reqType) {
    return true;
}

std::string c_FetchInterface::f_toString(c_Controller *p_ctrl,std::string p_message)  {
    return p_message;
}

void c_FetchInterface::f_watchDogTimer()  {
    for(t_Size l_index = 0; l_index < m_fetchRequestQueue.size(); l_index++) {
        if(m_clockCycle - m_fetchRequestQueue[l_index]->f_getInitTime() > m_coreConfig->s_coreTimeout) {
            m_fetchRequestQueue[l_index]->f_logError(this,"Fetch Q Request Watchdog Timeout");
        }
    }
}

bool c_FetchInterface::f_getModuleDone()   {
    return m_fetchRequestQueue.empty();
}

void c_FetchInterface::f_portCheck()  {
    m_readReqPort->f_portCheck();
    m_writeReqPort->f_portCheck();
    m_readRespPort->f_portCheck();
    m_probeReqPort->f_portCheck();
    m_writeDataPort->f_portCheck();
}
