#include "LoadStoreInterface.hpp"
#include "CoreConfig.hpp"
#include "SlavePort.hpp"
#include "MasterPort.hpp"
#include "RequestPool.hpp"
#include <assert.h>

c_LoadStoreInterface::c_LoadStoreInterface(c_CoreConfig *p_coreConfig) : 
        c_Controller(p_coreConfig->s_name)   {
    m_clockCycle = 0;
    m_coreConfig = p_coreConfig;

    m_readReqPort = new c_MasterPort(this,"coreRdReqPort",p_coreConfig->s_coreId,t_ResponseType::E_MISS);
    m_writeReqPort = new c_MasterPort(this,"coreWrReqPort",p_coreConfig->s_coreId,t_ResponseType::E_VICTIM);
    m_readRespPort = new c_SlavePort(this,"coreRdRespPort",p_coreConfig->s_coreId,t_RequestType::E_FILL);
    m_writeDataPort = new c_MasterPort(this,"coreWriteDataPort",p_coreConfig->s_coreId,t_ResponseType::E_PROBE_RESP_SEND);
    m_probeReqPort = new c_SlavePort(this,"coreProbeReqPort",p_coreConfig->s_coreId,t_RequestType::E_PROBE_REQ);

    m_loadsExecuted = 0;
    m_storesExecuted = 0;

    m_clockCycle = 0;
}

c_LoadStoreInterface::~c_LoadStoreInterface()   {
    delete m_readReqPort;
    delete m_writeReqPort;
    delete m_readRespPort;
    delete m_writeDataPort;
    delete m_probeReqPort;
}

void c_LoadStoreInterface::f_portCheck()  {
    m_readReqPort->f_portCheck();
    m_writeReqPort->f_portCheck();
    m_readRespPort->f_portCheck();
    m_writeDataPort->f_portCheck();
    m_probeReqPort->f_portCheck();
}

t_Time c_LoadStoreInterface::f_currentCycle() {
    return m_clockCycle;
}

void c_LoadStoreInterface::f_resetClock()	{
	m_clockCycle = 0;
}

void c_LoadStoreInterface::f_clockCycle()    {
    f_watchDogTimer();
    f_retire();
    f_execute();
    f_issue();
    f_decode();
    f_fetch();
    m_clockCycle++;
}

c_MasterPort* c_LoadStoreInterface::f_getReadReqPort()   {
    return m_readReqPort;
}

c_MasterPort* c_LoadStoreInterface::f_getWriteReqPort()  {
    return m_writeReqPort;
}

c_SlavePort* c_LoadStoreInterface::f_getReadRespPort()   {
    return m_readRespPort;
}

c_SlavePort* c_LoadStoreInterface::f_getProbeReqPort()  {
    return m_probeReqPort;
}

c_MasterPort* c_LoadStoreInterface::f_getWriteDataPort()   {
    return m_writeDataPort;
}

c_Transaction* c_LoadStoreInterface::f_retireTransaction(t_Credit p_portNo, t_ResponseType p_respType) {
    c_Transaction* l_transaction = NULL;
    if(m_rob.size() == m_coreConfig->s_robSize)    {
        return NULL;       
    }
    if(p_respType == t_ResponseType::E_MISS)    {
        if(!m_loadQueue.empty())    {
            l_transaction = m_loadQueue.front();
            m_rob.push_back(l_transaction);    
            m_loadQueue.erase(m_loadQueue.begin());
            l_transaction->f_logDebug(this,"Sending read request to memory");
        }
    }
    else if(p_respType == t_ResponseType::E_VICTIM)    {
        if(!m_storeQueue.empty())    {
            l_transaction = m_storeQueue.front();
            m_rob.push_back(l_transaction);
            m_storeQueue.erase(m_storeQueue.begin());
            l_transaction->f_logDebug(this,"Sending write request to memory");
        }
    }
    else if(p_respType == t_ResponseType::E_PROBE_RESP_SEND)    {
        if(!m_storeDataQueue.empty())    {
            l_transaction = m_storeDataQueue.front();
            l_transaction->f_logDebug(this,"Sending write data to memory");
            m_storeDataQueue.erase(m_storeDataQueue.begin());             
        }    
    }
    else    {
        f_logError(this,"Invalid transaction type");
    }
    assert(m_rob.size() <= m_coreConfig->s_robSize);    
    return l_transaction;
}

void c_LoadStoreInterface::f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction)  {
    auto l_indexToRemove = m_rob.end();
    for(auto l_index = m_rob.begin(); l_index != m_rob.end(); l_index++) {
        if(*l_index == p_transaction)    {
            if(p_transaction->f_getRequestType() == t_RequestType::E_FILL)    {
                p_transaction->f_logDebug(this,"Read Response received from memory");
                l_indexToRemove = l_index;
                m_loadsExecuted++;
                c_RequestPool<c_Transaction>::f_deAllocate(p_transaction);
                break;
            }
            else if(p_transaction->f_getRequestType() == t_RequestType::E_PROBE_REQ)    {
                p_transaction->f_logDebug(this,"Probe request received from memory");
                p_transaction->f_setRequestType(t_RequestType::E_PROBE_RESP);
                p_transaction->f_setResponseType(t_ResponseType::E_NUM_SENDQ_TYPE);
                p_transaction->f_setProbeType(t_ProbeType::E_NUM_PROBETYPE);
                p_transaction->f_setResponseCacheState(t_CacheState::E_MODIFIED);
                m_storeDataQueue.push_back(p_transaction);
                l_indexToRemove = l_index; 
                m_storesExecuted++;
                break;
            }
            else    {
                p_transaction->f_logError(this,"Unknown transaction type");
            }
        }
    }
    if(l_indexToRemove != m_rob.end())  {
        m_rob.erase(l_indexToRemove);
    }
    else    {
        p_transaction->f_logError(this,"Wrong destination: Should be in other core or FU");
    }
}

bool c_LoadStoreInterface::f_pushTransaction(c_Transaction *p_transaction)  {
    if(p_transaction->f_getRequestType() == t_RequestType::E_CORE_READ) {
        if(m_loadQueue.size() < m_coreConfig->s_loadQSize)  {
            p_transaction->f_setLineType(t_LineType::E_DATA);
            m_loadQueue.push_back(p_transaction);
            return true;
        }
        else    {
            return false;
        }
    }
    else if(p_transaction->f_getRequestType() == t_RequestType::E_CORE_WRITE) {
        if(m_storeQueue.size() < m_coreConfig->s_storeQSize)  {
            p_transaction->f_setLineType(t_LineType::E_DATA);
            m_storeQueue.push_back(p_transaction);
            return true;
        }
        else    {
            return false;
        }
    }
    return false;
}

void c_LoadStoreInterface::f_fetch()  {
    m_readRespPort->f_clockCycle();
    m_probeReqPort->f_clockCycle();
}

void c_LoadStoreInterface::f_decode() {}

void c_LoadStoreInterface::f_execute()    {}

void c_LoadStoreInterface::f_issue()  {}

void c_LoadStoreInterface::f_retire()  {
    m_readReqPort->f_clockCycle();
    m_writeReqPort->f_clockCycle();
    m_writeDataPort->f_clockCycle();
}

bool c_LoadStoreInterface::f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_reqType) {
    return true;
}

std::string c_LoadStoreInterface::f_toString(c_Controller *p_ctrl,std::string p_message)  {
    return p_message;
}

void c_LoadStoreInterface::f_watchDogTimer()  {
    for(t_Size l_index = 0; l_index < m_rob.size(); l_index++) {
        if(m_clockCycle - m_rob[l_index]->f_getInitTime() > m_coreConfig->s_coreTimeout) {
            m_rob[l_index]->f_logError(this,"Re-order Buffer Request Watchdog Timeout");
        }
    }

    for(t_Size l_index = 0; l_index < m_loadQueue.size(); l_index++) {
        if(m_clockCycle - m_loadQueue[l_index]->f_getInitTime() > m_coreConfig->s_coreTimeout) {
            m_loadQueue[l_index]->f_logError(this,"Read Q Request Watchdog Timeout");
        }
    }

    for(t_Size l_index = 0; l_index < m_storeQueue.size(); l_index++) {
        if(m_clockCycle - m_storeQueue[l_index]->f_getInitTime() > m_coreConfig->s_coreTimeout) {
            m_storeQueue[l_index]->f_logError(this,"Store Q Request Watchdog Timeout");
        }
    }

    for(t_Size l_index = 0; l_index < m_storeDataQueue.size(); l_index++) {
        if(m_clockCycle - m_storeDataQueue[l_index]->f_getInitTime() > m_coreConfig->s_coreTimeout) {
            m_storeDataQueue[l_index]->f_logError(this,"Store Data Q Request Watchdog Timeout");
        }
    }
}

bool c_LoadStoreInterface::f_getModuleDone()   {
    return m_rob.empty() && m_loadQueue.empty() && m_storeQueue.empty() && m_storeDataQueue.empty();
}
