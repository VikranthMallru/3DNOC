#include "FixedLatencyStorage.hpp"
#include "FixedLatencyStoragePacket.hpp"
#include "Transaction.hpp"
#include "types.hpp"
#include "MasterPort.hpp"
#include "RequestPool.hpp"
#include <queue>
#include <assert.h>

c_FixedLatencyStorage::c_FixedLatencyStorage(std::string p_name, c_StorageConfig *p_storageConfig, t_CoreID p_coreID, c_SimConfig* p_config): c_Controller(p_name) {
    m_size = p_storageConfig->s_size;
    m_coreID = p_coreID;
    m_offset = p_storageConfig->s_offset;
    m_fixedLatency = p_storageConfig->s_fixedLatency;
    m_coreRdCredit = p_storageConfig->s_readReqQSize;
    m_coreWrCredit = p_storageConfig->s_writeReqQSize;
    m_config = p_storageConfig;
    m_simConfig = p_config;
        
    m_memRdReqPort = new c_SlavePort(this,"READREQMEMPortMainMemory",0, t_RequestType::E_CORE_READ);
	m_memWrReqPort = new c_SlavePort(this,"WRITEREQMEMPortMainMemory",0, t_RequestType::E_CORE_WRITE);
    m_memRdRespPort = new c_MasterPort(this,"READRESPMEMPortMainMemory",0,t_ResponseType::E_READ_RESP); // Response type master port
    m_memPrbReqPort = new c_MasterPort(this,"PRBREQMEMPortMainMemory",0,t_ResponseType::E_PROBE_REQ_SEND); // Request type master port 
    m_memWrDataPort = new c_SlavePort(this,"WriteDataMEMPortMainMemory",0, t_RequestType::E_PROBE_RESP);

    m_actualDataStore = new unsigned char[m_size];
    m_clockCycle = 0;
    m_priorityRR = 0;
}

c_FixedLatencyStorage::~c_FixedLatencyStorage()  {
    delete m_memRdReqPort;
    delete m_memWrReqPort;
    delete m_memRdRespPort;
    delete m_memPrbReqPort;
    delete m_memWrDataPort;

    delete[] m_actualDataStore;
}

void c_FixedLatencyStorage::f_portCheck()  {
    m_memRdReqPort->f_portCheck();
    m_memWrReqPort->f_portCheck();
    m_memRdRespPort->f_portCheck();
    m_memPrbReqPort->f_portCheck();
    m_memWrDataPort->f_portCheck();
}

t_Time c_FixedLatencyStorage::f_currentCycle() {
    return m_clockCycle;
}

void c_FixedLatencyStorage::f_resetClock()	{
	m_clockCycle = 0;
}

void c_FixedLatencyStorage::f_clockCycle() {
    if(m_simConfig->s_activeMem[m_coreID]){
        f_retire();
        f_issue();
        f_execute();
        f_decode();
        f_fetch();
        m_clockCycle++;
    }
}

void c_FixedLatencyStorage::f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction)    {
    c_FixedLatencyStoragePacket *l_packet = c_RequestPool<c_FixedLatencyStoragePacket>::f_allocate(m_clockCycle);
    if(p_transaction->f_getRequestType() == t_RequestType::E_CORE_READ)    {
        p_transaction->f_logDebug(this,"Received Read in Memory");
        t_FixedLatencyRequestType l_reqType = convertToFixedLatencyRequestType(p_transaction->f_getRequestType());
        l_packet->f_initPacket(p_transaction,l_reqType,m_clockCycle);
        t_Address l_physAddr = l_packet->f_getStorageAddress();
        m_readQueue.push_back(l_packet);
    }
    else if(p_transaction->f_getRequestType() == t_RequestType::E_CORE_WRITE)    {
        p_transaction->f_logDebug(this,"Received Write Request in Memory");
        t_FixedLatencyRequestType l_reqType = convertToFixedLatencyRequestType(p_transaction->f_getRequestType());
        l_packet->f_initPacket(p_transaction,l_reqType,m_clockCycle);
        t_Address l_physAddr = l_packet->f_getStorageAddress();
        m_probeQueue.push_back(l_packet);
    }
    else if(p_transaction->f_getRequestType() == t_RequestType::E_PROBE_RESP)    {
        p_transaction->f_logDebug(this,"Received Write data in Memory");
        l_packet->f_initPacket(p_transaction,t_FixedLatencyRequestType::E_FIXED_LATENCY_DATA_WRITE,m_clockCycle);
        m_writeQueue.push_back(l_packet);
    }
    else    {
        p_transaction->f_logError(this,"Unknown COMMAND while fetching transaction");
    }
}

t_FixedLatencyRequestType c_FixedLatencyStorage::convertToFixedLatencyRequestType(t_RequestType p_reqType)  {
    if(p_reqType == t_RequestType::E_CORE_READ) {
        return t_FixedLatencyRequestType::E_FIXED_LATENCY_DATA_READ;
    }
    else if(p_reqType == t_RequestType::E_CORE_WRITE) {
        return t_FixedLatencyRequestType::E_FIXED_LATENCY_DATA_WRITE;        
    }
    else if(p_reqType == t_RequestType::E_PROBE_RESP)    {
        return t_FixedLatencyRequestType::E_FIXED_LATENCY_DATA_WRITE;        
    }
    else    {
        f_logError(this,"Unknown COMMAND while converting packets");
        return t_FixedLatencyRequestType::E_NUM_FIXED_LATENCY_REQTYPE;
    }
}

c_Transaction* c_FixedLatencyStorage::f_retireTransaction(t_PortNo p_portNo, t_ResponseType p_responseType)    {
    if(p_responseType == t_ResponseType::E_PROBE_REQ_SEND)    {
        if(!m_probeQueue.empty())   {
            c_FixedLatencyStoragePacket *l_packet = m_probeQueue.front();
            m_probeQueue.erase(m_probeQueue.begin());
            c_Transaction *l_transaction = l_packet->f_getTransaction();
            l_transaction->f_setRequestType(t_RequestType::E_NUM_RECVQ_TYPE);
            l_transaction->f_setResponseType(t_ResponseType::E_PROBE_REQ_SEND);
            l_transaction->f_logDebug(this,"Sending a probe to get the data and deleting packet");
            c_RequestPool<c_FixedLatencyStoragePacket>::f_deAllocate(l_packet);
            return l_transaction;
        }
    }
    else if(p_responseType == t_ResponseType::E_READ_RESP)    {
        if(!m_readQueue.empty())    {
            c_FixedLatencyStoragePacket *l_packet = (c_FixedLatencyStoragePacket *) m_readQueue.front();
            t_Time l_reqTime = l_packet->f_getReqTime();
            if(m_clockCycle > (l_reqTime + m_fixedLatency))   {
                c_Transaction* l_transaction = l_packet->f_getTransaction();
                    l_transaction->f_setRequestType(t_RequestType::E_FILL);
                    l_transaction->f_setResponseType(t_ResponseType::E_NUM_SENDQ_TYPE);
                    if(l_transaction->f_getChangeToXType() == t_ChangeToXType::E_C2X_REQ)  {
                        l_transaction->f_setChangeToXType(t_ChangeToXType::E_C2X_RESP);
                    }

                    l_transaction->f_setResponseCacheState(t_CacheState::E_EXCLUSIVE);
                    m_readQueue.erase(m_readQueue.begin());
                    l_transaction->f_logDebug(this,"Sending read response and deleting packet");
                    c_RequestPool<c_FixedLatencyStoragePacket>::f_deAllocate(l_packet);
                    return l_transaction;
            }          
        }
    }
    else    {
        f_logError(this,"Unknown COMMAND whlle finding transaction to retire");
    }
    return NULL;
}

c_MasterPort* c_FixedLatencyStorage::f_getResponsePort(t_ResponseType p_responseType)    {
    switch(p_responseType)  {
    case t_ResponseType::E_PROBE_REQ_SEND:
        return m_memPrbReqPort;
    case t_ResponseType::E_READ_RESP:
        return m_memRdRespPort;
    default:
        assert(false);
    }
}

c_SlavePort* c_FixedLatencyStorage::f_getRequestPort(t_RequestType p_requestType)    {
    switch(p_requestType)   {
    case t_RequestType::E_CORE_READ:
        return m_memRdReqPort;
    case t_RequestType::E_CORE_WRITE:
        return m_memWrReqPort;
    case t_RequestType::E_PROBE_RESP:
        return m_memWrDataPort;
    default:
        assert(false);
    }
}

void c_FixedLatencyStorage::f_fetch()   {
    m_memRdReqPort->f_clockCycle();
    m_memWrReqPort->f_clockCycle();
    m_memWrDataPort->f_clockCycle();
}

void c_FixedLatencyStorage::f_decode()   {
}

void c_FixedLatencyStorage::f_retire()   {
    m_memRdRespPort->f_clockCycle();
    m_memPrbReqPort->f_clockCycle();
}

void c_FixedLatencyStorage::f_execute()   {
    if(m_priorityRR and !m_readQueue.empty() && m_readQueue.front()->f_getReqType() == t_FixedLatencyRequestType::E_FIXED_LATENCY_DATA_READ)    {
        c_FixedLatencyStoragePacket *l_packet = m_readQueue.front();
        c_Transaction* l_transaction = l_packet->f_getTransaction();
        t_Address l_physAddr = l_packet->f_getStorageAddress();
        l_transaction->f_logDebug(this,"Done Reading");
        //std::memcpy(l_transaction->f_getData(), m_actualDataStore + l_physAddr - m_offset, l_transaction->f_getDataSize());
    }
    else if(not m_priorityRR and !m_writeQueue.empty() && m_writeQueue.front()->f_getReqType() == t_FixedLatencyRequestType::E_FIXED_LATENCY_DATA_WRITE)    {
        c_FixedLatencyStoragePacket *l_packet = m_writeQueue.front();
        c_Transaction* l_transaction = l_packet->f_getTransaction();
        l_transaction->f_logDebug(this,"Done writing in memory deleting packet");
        t_Address l_physAddr = l_packet->f_getStorageAddress();
        m_writeQueue.erase(m_writeQueue.begin());
        c_RequestPool<c_Transaction>::f_deAllocate(l_transaction);
        c_RequestPool<c_FixedLatencyStoragePacket>::f_deAllocate(l_packet);
        //std::memcpy(m_actualDataStore + l_physAddr - m_offset, l_transaction->f_getData(), l_transaction->f_getDataSize());
    }
    m_priorityRR ^= 1;
}

void c_FixedLatencyStorage::f_issue()   {
}

std::string c_FixedLatencyStorage::f_toString(c_Controller *p_ctrl,std::string p_message)  {
    return p_message;
}

bool c_FixedLatencyStorage::f_getModuleDone(){
    return m_readQueue.empty() && m_probeQueue.empty() && m_writeQueue.empty();
}

bool c_FixedLatencyStorage::f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_reqType) {
    if(p_reqType == t_RequestType::E_CORE_READ) {
        return (m_coreRdCredit - m_readQueue.size() > 0);
    }
    else if(p_reqType == t_RequestType::E_CORE_WRITE) {
        return (m_coreWrCredit - m_probeQueue.size() > 0);
    }
    else if(p_reqType == t_RequestType::E_PROBE_RESP) {
        return (m_coreWrCredit - m_writeQueue.size() > 0);
    }
    else    {
        f_logError(this,"Unknown port accessed");
        return false;
    }
}

void c_FixedLatencyStorage::f_watchDogTimer()  {
    for(t_Size l_index = 0; l_index < m_readQueue.size(); l_index++) {
        if(m_clockCycle - m_readQueue[l_index]->f_getTransaction()->f_getInitTime() > m_config->s_storageTimeout) {
            m_readQueue[l_index]->f_logError(this,"Read Q Request Watchdog Timeout");
        }
    }

    for(t_Size l_index = 0; l_index < m_writeQueue.size(); l_index++) {
        if(m_clockCycle - m_writeQueue[l_index]->f_getTransaction()->f_getInitTime() > m_config->s_storageTimeout) {
            m_writeQueue[l_index]->f_logError(this,"Store Q Request Watchdog Timeout");
        }
    }

    for(t_Size l_index = 0; l_index < m_probeQueue.size(); l_index++) {
        if(m_clockCycle - m_probeQueue[l_index]->f_getTransaction()->f_getInitTime() > m_config->s_storageTimeout) {
            m_probeQueue[l_index]->f_logError(this,"Probe Q Request Watchdog Timeout");
        }
    }
}

bool c_FixedLatencyStorage::f_pushTransaction(c_Transaction *p_transaction)  {
    if(p_transaction->f_getRequestType() == t_RequestType::E_PROBE_REQ) {
        if(m_coreWrCredit - m_probeQueue.size() > 0)  {
            c_FixedLatencyStoragePacket *l_packet = c_RequestPool<c_FixedLatencyStoragePacket>::f_allocate(m_clockCycle);
            t_FixedLatencyRequestType l_reqType = convertToFixedLatencyRequestType(p_transaction->f_getRequestType());
            l_packet->f_initPacket(p_transaction,l_reqType,m_clockCycle);
            m_probeQueue.push_back(l_packet);
            return true;
        }
        else    {
            return false;
        }
    }
    return false;
}
