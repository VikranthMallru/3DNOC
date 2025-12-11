#include "MultiCacheNetworkInterface.hpp"
#include "SimConfig.hpp"
#include "Packet.hpp"
#include "Controller.hpp"
#include "RequestPool.hpp"


c_MultiCacheNetworkInterface::c_MultiCacheNetworkInterface(std::string p_moduleName, t_CoreID p_coreId, c_SimConfig *p_simConfig, c_CoreConfig *p_coreConfig) : c_Controller(p_moduleName)   {
    m_clockCycle = 0;
	m_ctrl = this;
	m_coreConfig = p_coreConfig;
	m_coreId = p_coreId;
    for(t_Size l_reqType = 0; l_reqType < (t_Size) t_RequestType::E_PREFETCH_RECV; l_reqType++)	{
		m_lastPort[l_reqType] = 0;
        if(l_reqType == E_CORE_READ || l_reqType == E_CORE_WRITE || l_reqType == E_PROBE_RESP)	{
			m_numRecvPort[l_reqType] = p_simConfig->s_maxPrivateCacheTypes;
		}
		else {
			m_numRecvPort[l_reqType] = 1;
		}
		m_recvQSize[l_reqType] = 1;
		m_recvQ[l_reqType] = new c_Queue*[m_numRecvPort[l_reqType]];
		m_recvPorts[l_reqType] = new c_SlavePort*[m_numRecvPort[l_reqType]];
        for(t_PortNo l_portNo = 0; l_portNo < m_numRecvPort[l_reqType]; l_portNo++)	{
        	m_recvQ[l_reqType][l_portNo] = new c_Queue(c_TypeToString::f_requestTypeString((t_RequestType)l_reqType),(t_RequestType)l_reqType, t_ResponseType::E_NUM_SENDQ_TYPE, l_portNo, m_recvQSize[l_reqType],t_QueueType::E_REQ_Q);
			std::string l_name = p_moduleName + "-recvPortCache-reqType-" + c_TypeToString::f_requestTypeString((t_RequestType)l_reqType) + "-PortNo-" + std::to_string(l_portNo);
            m_recvPorts[l_reqType][l_portNo] = new c_SlavePort(this, l_name,l_portNo, (t_RequestType) l_reqType);
        }
    }

    for(t_Size l_respType = 0; l_respType < (t_Size) t_ResponseType::E_NUM_SENDQ_TYPE; l_respType++)	{
		if(l_respType == E_PROBE_REQ_SEND || l_respType == E_READ_RESP)	{
			m_numSendPort[l_respType] = p_simConfig->s_maxPrivateCacheTypes;
		}
		else {
        	m_numSendPort[l_respType] = 1;
		}
		m_sendQSize[l_respType] = 1;
        m_sendQ[l_respType] = new c_Queue*[m_numSendPort[l_respType]];
		m_sendPorts[l_respType] = new c_MasterPort*[m_numSendPort[l_respType]];
		for(t_PortNo l_portNo = 0; l_portNo < m_numSendPort[l_respType]; l_portNo++)	{
            m_sendQ[l_respType][l_portNo] = new c_Queue(c_TypeToString::f_responseTypeString((t_ResponseType)l_respType),t_RequestType::E_NUM_RECVQ_TYPE,(t_ResponseType)l_respType,l_portNo,m_sendQSize[l_respType],t_QueueType::E_RESP_Q);
			std::string l_name = p_moduleName + "-sendPortCache-respType-" + c_TypeToString::f_responseTypeString((t_ResponseType)l_respType) + "-PortNo-" + std::to_string(l_portNo);
            m_sendPorts[l_respType][l_portNo] = new c_MasterPort(this,l_name,l_portNo,(t_ResponseType)l_respType);			
        }
    }
}

c_MultiCacheNetworkInterface::~c_MultiCacheNetworkInterface()   {
    for(t_Size l_reqType = 0; l_reqType < (t_Size) t_RequestType::E_PREFETCH_RECV; l_reqType++)	{
		for(t_PortNo l_portNo = 0; l_portNo < m_numRecvPort[l_reqType]; l_portNo++)	{
			delete m_recvQ[l_reqType][l_portNo];
			delete m_recvPorts[l_reqType][l_portNo];
		}
		delete[] m_recvQ[l_reqType];
		delete[] m_recvPorts[l_reqType];
	}
    for(t_Size l_respType = 0; l_respType < (t_Size) t_ResponseType::E_NUM_SENDQ_TYPE; l_respType++)	{
		for(t_PortNo l_portNo = 0; l_portNo < m_numSendPort[l_respType]; l_portNo++)	{
			delete m_sendQ[l_respType][l_portNo];
			delete m_sendPorts[l_respType][l_portNo];			
		}
		delete[] m_sendQ[l_respType];
		delete[] m_sendPorts[l_respType];
	}
}

void c_MultiCacheNetworkInterface::f_clockCycle() {
    try	{
		f_watchDogTimer();
		f_retire();
		f_execute();
		f_issue();
		f_decode();
		f_fetch();
		m_clockCycle++;
	}
	catch(const std::exception& e)	{
		std::cerr << 'Error in clock cycle of the cache controller\n';
	}
}

t_Time c_MultiCacheNetworkInterface::f_currentCycle() {
    return m_clockCycle;
}

void c_MultiCacheNetworkInterface::f_resetClock() {
    m_clockCycle = 0;
}

void c_MultiCacheNetworkInterface::f_fetch()  {
    try {
		for(t_Size l_reqType = 0; l_reqType < (t_Size) t_RequestType::E_PREFETCH_RECV; l_reqType++)	{
			for(t_PortNo l_portNo = 0; l_portNo < m_numRecvPort[l_reqType]; l_portNo++)	{
				m_recvPorts[l_reqType][l_portNo]->f_clockCycle();
			}
		}
	}
	catch(const std::exception& e)    {
        std::cerr<<"Error in fetching request"<<std::endl;
        exit(-1);
    }
}

void c_MultiCacheNetworkInterface::f_fetchTransaction(t_PortNo p_portNo,c_Transaction *p_transaction)	{
	try {
		c_Packet *l_packet = c_RequestPool<c_Packet>::f_allocate(m_clockCycle);
		l_packet->f_initPacket(p_portNo);
		l_packet->f_setTransaction(p_transaction);
		l_packet->f_setRequestControllerState(t_RequestControllerState::E_RCS_FETCHED);
		l_packet->f_setFetchTime(m_ctrl->f_currentCycle());
		p_transaction->f_addPacket(l_packet);
		p_transaction->f_setCurrentLevel(M_CORE_NOC_LEVEL);
		m_recvQ[p_transaction->f_getRequestType()][p_portNo]->f_allocate(l_packet);
		p_transaction->f_logDebug(m_ctrl,"Request fetched");
	}
	catch(const std::exception& e)    {
        p_transaction->f_logError(m_ctrl,"Error fetching the transaction");
    }
}

c_Transaction* c_MultiCacheNetworkInterface::f_retireTransaction(t_PortNo p_portNo, t_ResponseType p_respType)	{
	if(!m_sendQ[p_respType][p_portNo]->f_empty())	{
		c_Packet *l_packet = (c_Packet*)(m_sendQ[p_respType][p_portNo])->f_get(0);
		c_Transaction *l_transaction = l_packet->f_getTransaction();
		l_transaction->f_logDebug(m_ctrl,"Request serviced");
		l_transaction->f_removePacket(l_packet);
		m_sendQ[p_respType][p_portNo]->f_deallocate(l_packet);
		c_RequestPool<c_Packet>::f_deAllocate(l_packet);
		return l_transaction;
	}
	return NULL;
}

void c_MultiCacheNetworkInterface::f_decode() {}

void c_MultiCacheNetworkInterface::f_issue()  {
	bool *l_outputDone[t_ResponseType::E_NUM_SENDQ_TYPE];
	for(t_Size l_respType = 0; l_respType < (t_Size) t_ResponseType::E_NUM_SENDQ_TYPE; l_respType++)	{
		l_outputDone[l_respType] = new bool[m_numSendPort[l_respType]];
		for(t_PortNo l_portNo = 0; l_portNo < m_numSendPort[l_respType]; l_portNo++)	{
			l_outputDone[l_respType][l_portNo] = false;
		}
	}
	for(t_Size l_reqType = 0; l_reqType < (t_Size) t_RequestType::E_PREFETCH_RECV; l_reqType++)	{
		for(t_PortNo l_portNo = 0; l_portNo < m_numRecvPort[l_reqType]; l_portNo++)	{
			t_ResponseType l_responseType;
			switch(l_reqType)	{
			case t_RequestType::E_CORE_READ:
				l_responseType = t_ResponseType::E_MISS;
				break;
			case t_RequestType::E_CORE_WRITE:
				l_responseType = t_ResponseType::E_VICTIM;
				break;
			case t_RequestType::E_FILL:
				l_responseType = t_ResponseType::E_READ_RESP;
				break;
			case t_RequestType::E_PROBE_REQ:
				l_responseType = t_ResponseType::E_PROBE_REQ_SEND;
				break;
			case t_RequestType::E_PROBE_RESP:
				l_responseType = t_ResponseType::E_PROBE_RESP_SEND;
				break;
			case t_RequestType::E_PREFETCH_RECV:
				l_responseType = t_ResponseType::E_PREFETCH_SEND;
				break;
			default:
				break;	
			}
			t_PortNo l_portNoRR = (l_portNo + m_lastPort[l_reqType] + 1) % m_numRecvPort[l_reqType]; 
			if(!m_recvQ[l_reqType][l_portNoRR]->f_empty())	{
				c_Packet *l_packet = (c_Packet*) m_recvQ[l_reqType][l_portNoRR]->f_get(0);
				c_Transaction *l_transaction = l_packet->f_getTransaction();
				t_LineType l_lineType = l_transaction->f_getLineType();
				if(l_responseType != E_PROBE_REQ_SEND && l_responseType != E_READ_RESP)	{
					l_lineType = E_INSTRUCTION;
				}
				if(!l_outputDone[l_responseType][l_lineType] && m_sendQ[l_responseType][l_lineType]->f_hasCredit())	{
					m_recvQ[l_reqType][l_portNoRR]->f_deallocate(l_packet);
					m_sendQ[l_responseType][l_lineType]->f_allocate(l_packet);
					l_outputDone[l_responseType][l_lineType] = true;
					m_lastPort[l_reqType] = l_portNo;
				}
			}
		}
	}

	for(t_Size l_respType = 0; l_respType < (t_Size) t_ResponseType::E_NUM_SENDQ_TYPE; l_respType++)	{
		delete[] l_outputDone[l_respType];
	}
}

void c_MultiCacheNetworkInterface::f_execute()    {}

void c_MultiCacheNetworkInterface::f_retire() {
	try {
		for(t_Size l_respType = 0; l_respType < (t_Size) t_ResponseType::E_NUM_SENDQ_TYPE; l_respType++)	{
			for(t_PortNo l_portNo = 0; l_portNo < m_numSendPort[l_respType]; l_portNo++)	{
				m_sendPorts[l_respType][l_portNo]->f_clockCycle();
			}
		}
	}
	catch(const std::exception& e)    {
        std::cerr<<"Error in fetching request"<<std::endl;
        exit(-1);
    }
}

std::string c_MultiCacheNetworkInterface::f_pipeString()  {
    std::string l_message;
    return l_message;
}

c_SlavePort* c_MultiCacheNetworkInterface::f_getRequestPort(t_RequestType p_requestType,t_PortNo p_portNo)    {
    return m_recvPorts[p_requestType][p_portNo];
}

c_MasterPort* c_MultiCacheNetworkInterface::f_getResponsePort(t_ResponseType p_responseType,t_PortNo p_portNo)    {
    return m_sendPorts[p_responseType][p_portNo];
}

void c_MultiCacheNetworkInterface::f_portCheck()  {
    for(t_Size l_reqType = 0; l_reqType < (t_Size) t_RequestType::E_PREFETCH_RECV; l_reqType++)	{
		for(t_PortNo l_portNo = 0; l_portNo < m_numRecvPort[l_reqType]; l_portNo++)	{
			m_recvPorts[l_reqType][l_portNo]->f_portCheck();			
		}
	}
    for(t_Size l_respType = 0; l_respType < (t_Size) t_ResponseType::E_PREFETCH_SEND; l_respType++)	{
		for(t_PortNo l_portNo = 0; l_portNo < m_numSendPort[l_respType]; l_portNo++)	{
			m_sendPorts[l_respType][l_portNo]->f_portCheck();			
		}
	}
}

bool c_MultiCacheNetworkInterface::f_getModuleDone()	{
	try {
		bool l_done = true;
		for(t_Size l_reqType = 0; l_reqType < (t_Size) t_RequestType::E_PREFETCH_RECV; l_reqType++)	{
			for(t_PortNo l_portNo = 0; l_portNo < m_numRecvPort[l_reqType]; l_portNo++)	{
				l_done &= m_recvQ[l_reqType][l_portNo]->f_empty();
			}
		}
		for(t_Size l_respType = 0; l_respType < (t_Size) t_ResponseType::E_NUM_SENDQ_TYPE; l_respType++)	{
			for(t_PortNo l_portNo = 0; l_portNo < m_numSendPort[l_respType]; l_portNo++)	{
				l_done &= m_sendQ[l_respType][l_portNo]->f_empty();
			}
		}
		return l_done;
	}
	catch(const std::exception& e)    {
        std::cerr<<"Error in fetching request"<<std::endl;
        exit(-1);
    }
}

bool c_MultiCacheNetworkInterface::f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType)	{
	return m_recvQ[p_nextReqType][p_portNo]->f_hasCredit();
}

void c_MultiCacheNetworkInterface::f_watchDogTimer()	{
	try {
		for(t_Size l_reqType = 0; l_reqType < (t_Size) t_RequestType::E_PREFETCH_RECV; l_reqType++)	{
			for(t_PortNo l_portNo = 0; l_portNo < m_numRecvPort[l_reqType]; l_portNo++)	{
				for(t_Size l_index = 0; l_index < m_recvQ[l_reqType][l_portNo]->f_size(); l_index++) {
					if(m_ctrl->f_currentCycle() - ((c_Packet*) m_recvQ[l_reqType][l_portNo]->f_get(l_index))->f_getRequestTime() > m_coreConfig->s_coreTimeout) {
						((c_Packet*) m_recvQ[l_reqType][l_portNo]->f_get(l_index))->f_getTransaction()->f_logError(m_ctrl,"Recv Q Request Watchdog Timeout");
					}
				}
			}
		}
		for(t_Size l_respType = 0; l_respType < (t_Size) t_ResponseType::E_NUM_SENDQ_TYPE; l_respType++)	{
			for(t_PortNo l_portNo = 0; l_portNo < m_numSendPort[l_respType]; l_portNo++)	{
				for(t_Size l_index = 0; l_index < m_sendQ[l_respType][l_portNo]->f_size(); l_index++) {
					if(m_ctrl->f_currentCycle() - ((c_Packet*) m_sendQ[l_respType][l_portNo]->f_get(l_index))->f_getRequestTime() > m_coreConfig->s_coreTimeout) {
						((c_Packet*) m_sendQ[l_respType][l_portNo]->f_get(l_index))->f_getTransaction()->f_logError(m_ctrl,"Send Q Request Watchdog Timeout");
					}
				}
			}
		}
	}
	catch(const std::exception& e)    {
        std::cerr<<"Error in fetching request"<<std::endl;
        exit(-1);
    }
}
