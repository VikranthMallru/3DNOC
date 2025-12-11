#include "Transaction.hpp"
#include "Packet.hpp"
#include "NetworkPacket.hpp"
#include "DirectoryPacket.hpp"
#include "TypeToString.hpp"
#include "Controller.hpp"
#include <iomanip>

t_Counter c_Transaction::m_transactionIdGenerator = 0;

c_Transaction::c_Transaction(){}

void c_Transaction::f_initTransaction(t_CoreID p_numCores,t_VirtualAddress p_virtAddr,t_RequestType p_reqType,
		 t_Data p_data, t_ResponseType p_responseType, t_ProbeType p_probeType,
		 t_CacheLevel p_installLevel, t_CacheLevel p_currentLevel, t_CacheState p_reqCacheState, t_Time p_initTime, t_CoreID p_coreId)	{
	m_numCores = p_numCores;
	m_virtAddr = p_virtAddr;
	m_reqType = p_reqType;
	m_reqCacheState = p_reqCacheState;
	m_respCacheState = E_INVALID;
	m_transactionId = m_transactionIdGenerator;
	m_phyAddr = p_virtAddr;
	m_dataSize = M_OFFSET_MAX;
	m_responseType = p_responseType;
	m_lineType = E_NOLINETYPE;
	m_probeType = p_probeType;
	m_changeToXType = E_NO_C2X;
	if(p_reqType == t_RequestType::E_CORE_READ)	{
		m_data = NULL;//new unsigned char[m_dataSize];
	}
	else	{
		m_data = p_data;
	}
	m_currentLevel = p_currentLevel;
	m_installLevel = p_installLevel;
	m_initTime = p_initTime;
	m_isPrefetch = false;
	m_cachePacket = NULL;
	m_networkPacket = NULL;
	m_directoryPacket = NULL;
	m_requestingCore = p_coreId;
	m_isFillNoReply = false;
	m_probeDestinations = new bool[m_numCores];
	for(t_Size l_destination = 0; l_destination < m_numCores; l_destination++)	{
		m_probeDestinations[l_destination] = false;
	}
	m_probeDataSource = M_MAX_NODE;
	m_replacedTagPreviousLevel = M_TAG_MAX;
	m_replacedWayPreviousLevel = M_WAY_MAX;
	m_transactionIdGenerator++;
}

void c_Transaction::f_copyTransaction(c_Transaction *p_transaction)	{
	m_numCores = p_transaction->f_getNumCores();
	m_virtAddr = p_transaction->f_getVirtualAddress();
	m_reqType = p_transaction->f_getRequestType();
	m_reqCacheState = p_transaction->f_getRequestCacheState();
	m_respCacheState = p_transaction->f_getResponseCacheState();
	m_transactionId = m_transactionIdGenerator;
	m_phyAddr = p_transaction->f_getPhysicalAddress();
	m_dataSize = p_transaction->f_getDataSize();
	m_responseType = p_transaction->f_getResponseType();
	m_lineType = p_transaction->f_getLineType();
	m_changeToXType = p_transaction->f_getChangeToXType();
	m_data = p_transaction->f_getData();
	m_currentLevel = p_transaction->f_getCurrentLevel();
	m_installLevel = p_transaction->f_getInstallLevel();
	m_initTime = p_transaction->f_getInitTime();
	m_isPrefetch = p_transaction->f_isPrefetch();
	m_probeType = p_transaction->f_getProbeType();
	m_requestingCore = p_transaction->f_getRequestingCore();
	m_cachePacket = NULL;
	m_networkPacket = NULL;
	m_directoryPacket = NULL;
	m_isFillNoReply = p_transaction->f_isFillNoReply();
	m_probeDestinations = new bool[m_numCores];
	for(t_Size l_destination = 0; l_destination < m_numCores; l_destination++)	{
		m_probeDestinations[l_destination] = p_transaction->f_isProbeDestination(l_destination);
	}
	m_replacedTagPreviousLevel = p_transaction->f_getReplacedTagPreviousLevel();
	m_replacedWayPreviousLevel = p_transaction->f_getReplacedWayPreviousLevel();
	m_probeDataSource = p_transaction->f_getProbeDataSource();
	m_transactionIdGenerator++;
}

t_Time c_Transaction::f_getInitTime()	{
	return m_initTime;
}

void c_Transaction::f_reset()	{
	m_numCores = M_MAX_NODE;
	m_virtAddr = M_OFFSET_MAX;
	m_reqCacheState = t_CacheState::E_INVALID;
	m_respCacheState = t_CacheState::E_INVALID;
	m_transactionId = 0;
	m_phyAddr = M_OFFSET_MAX;
	m_dataSize = M_OFFSET_MAX;
	m_responseType = t_ResponseType::E_NUM_SENDQ_TYPE;
	m_lineType = t_LineType::E_NOLINETYPE;
	m_probeType = t_ProbeType::E_NUM_PROBETYPE;
	m_changeToXType = t_ChangeToXType::E_NO_C2X;
	m_currentLevel = M_INSTALL_LEVEL_MAX;
	m_installLevel = M_INSTALL_LEVEL_MAX;
	m_initTime = 0;
	m_isPrefetch = false;
	m_cachePacket = NULL;
	m_requestingCore = M_CORE_MAX;
	m_directoryPacket = NULL;
	if(m_reqType == t_RequestType::E_CORE_READ)	{
		delete m_data;
	}
	delete m_probeDestinations;
	m_replacedTagPreviousLevel = M_TAG_MAX;
	m_replacedWayPreviousLevel = M_WAY_MAX;
	m_probeDataSource = M_MAX_NODE;
	m_isFillNoReply = false;
	m_reqType = t_RequestType::E_NUM_RECVQ_TYPE;
	m_data = NULL;
}

c_Transaction::~c_Transaction()	{
}

void c_Transaction::f_setProbeDestination(t_Size p_probeDestination)	{
	m_probeDestinations[p_probeDestination] = true;
}

bool c_Transaction::f_isProbeDestination(t_Size p_probeDestination)	{
	return m_probeDestinations[p_probeDestination];
}

void c_Transaction::f_setProbeDataSource(t_CoreID p_dataSource)	{
	m_probeDataSource = p_dataSource;
}

t_CoreID c_Transaction::f_getProbeDataSource()	{
	return m_probeDataSource;
}

void c_Transaction::f_addPacket(c_Packet *p_packet)	{
	m_cachePacket = p_packet;
}

void c_Transaction::f_addNetworkPacket(c_NetworkPacket *p_networkPacket)	{
	m_networkPacket = p_networkPacket;
}

void c_Transaction::f_addDirectoryPacket(c_DirectoryPacket* p_directoryPacket)   {
    m_directoryPacket = p_directoryPacket;
}

void c_Transaction::f_removePacket(c_Packet *p_packet)	{
	if(m_cachePacket != p_packet)	{
		m_cachePacket->f_logDebug(NULL,"Error removing packet from transaction");
		p_packet->f_logError(NULL,"Error removing packet from transaction");
	}
	m_cachePacket = NULL;
}

void c_Transaction::f_removeNetworkPacket(c_NetworkPacket *p_networkPacket)	{
	if(p_networkPacket != m_networkPacket)	{
		m_networkPacket->f_logDebug(NULL,"Error removing packet from transaction");
		p_networkPacket->f_logError(NULL,"Error removing packet from transaction");
	}
	m_networkPacket = NULL;
}

void c_Transaction::f_removeDirectoryPacket(c_DirectoryPacket *p_directoryPacket)	{
	if(p_directoryPacket != p_directoryPacket)	{
		m_directoryPacket->f_logDebug(NULL,"Error removing packet from transaction");
		p_directoryPacket->f_logError(NULL,"Error removing packet from transaction");
	}
	m_directoryPacket = NULL;
}

c_Packet* c_Transaction::f_getPacket()	{
	return m_cachePacket;
}


c_DirectoryPacket* c_Transaction::f_getDirectoryPacket()   {
    return m_directoryPacket;
}

t_Data c_Transaction::f_getData()	{
	return m_data;
}

t_Size c_Transaction::f_getDataSize()	{
	return m_dataSize;
}

t_CacheLevel c_Transaction::f_getInstallLevel()	{
	return m_installLevel;
}

t_CacheLevel c_Transaction::f_getCurrentLevel()	{
	return m_currentLevel;
}

t_PhysicalAddress c_Transaction::f_getPhysicalAddress()	{
	return m_phyAddr;
}

t_VirtualAddress c_Transaction::f_getVirtualAddress()	{
	return m_virtAddr;
}

t_RequestType c_Transaction::f_getRequestType()	{
	return m_reqType;
}

t_ResponseType c_Transaction::f_getResponseType()	{
	return m_responseType;
}

void c_Transaction::f_setCurrentLevel(t_CacheLevel p_currentLevel)	{
	m_currentLevel = p_currentLevel;
}

void c_Transaction::f_isPrefetch(bool p_isPrefetch)    {
    m_isPrefetch = p_isPrefetch;
}

bool c_Transaction::f_isPrefetch()    {
    return m_isPrefetch;
}

void c_Transaction::f_locate(t_Stack p_stack, t_Slice p_slice)	{
}

t_CacheState c_Transaction::f_getRequestCacheState()	{
	return m_reqCacheState;
}

void c_Transaction::f_setRequestCacheState(t_CacheState p_state)	{
	m_reqCacheState = p_state;
}

t_CacheState c_Transaction::f_getResponseCacheState()	{
	return m_respCacheState;
}

void c_Transaction::f_setResponseCacheState(t_CacheState p_state)	{
	m_respCacheState = p_state;
}

t_CoreID c_Transaction::f_getRequestingCore()	{
	return m_requestingCore;
}

void c_Transaction::f_setRequestingCore(t_CoreID p_coreId)	{
	m_requestingCore = p_coreId;
}

void c_Transaction::f_setPhysicalAddress(t_PhysicalAddress p_phyAddr)	{
	m_phyAddr = p_phyAddr;
}
	
void c_Transaction::f_setVirtualAddress(t_VirtualAddress p_virtAddr)	{
	m_virtAddr = p_virtAddr;
}


std::string c_Transaction::f_toString(c_Controller *p_ctrl,std::string p_message)	{
	std::stringstream m_PAStream;
    m_PAStream<<std::hex<<m_phyAddr;
    std::stringstream m_VAStream;
    m_VAStream<<std::hex<<m_virtAddr;
	std::string l_time;
	if(p_ctrl != NULL)	{
		l_time = std::to_string(p_ctrl->f_currentCycle());
	}
	else	{
		l_time = "NA";
	}
	std::stringstream l_cachePacketString;
	if(m_cachePacket != NULL)	{
		l_cachePacketString << m_cachePacket->f_toString(p_ctrl,"");
	} 
	else	{
		l_cachePacketString << "";
	}
	std::stringstream l_directoryPacketString;
	if(m_directoryPacket != NULL)	{
		l_directoryPacketString << m_directoryPacket->f_toString(p_ctrl,"");
	} 
	else	{
		l_directoryPacketString << "";
	}
	std::stringstream l_networkPacketString;
	if(m_networkPacket != NULL)	{
		l_networkPacketString << m_networkPacket->f_toString(p_ctrl,"");
	} 
	else	{
		l_networkPacketString << "";
	} 
	std::string l_message = l_time + "\t" + m_VAStream.str() + "\t" + std::to_string(m_transactionId) + "\t" + m_PAStream.str() + "\t" + std::to_string(m_requestingCore) +
	"\t" + c_TypeToString::f_lineTypeString(m_lineType) +
	"\t" + c_TypeToString::f_stateTypeString(m_reqCacheState) + "\t" + c_TypeToString::f_stateTypeString(m_respCacheState) +  
	"\t" + c_TypeToString::f_requestTypeString(m_reqType) + "\t" + c_TypeToString::f_responseTypeString(m_responseType) + 
	"\t" + c_TypeToString::f_probeTypeString(m_probeType) + "\t" + c_TypeToString::f_changeToXTypeString(m_changeToXType) +
    "\t" + std::to_string(m_currentLevel) + "\t" + std::to_string(m_installLevel) +
	"\t" + (m_isPrefetch ? "Y\t" : "N\t") + std::to_string(m_replacedTagPreviousLevel) + "\t" + std::to_string(m_replacedWayPreviousLevel) +
	"\tC:" + l_cachePacketString.str() +
	"\tD:" + l_directoryPacketString.str() +
	"\tN:" + l_networkPacketString.str() +
	"\tM:" + p_message + "\n";
	return l_message;
}

void c_Transaction::f_setRequestType(t_RequestType p_requestType)	{
	m_reqType = p_requestType;
}

void c_Transaction::f_setResponseType(t_ResponseType p_responseType)	{
	m_responseType = p_responseType;
}

t_Counter c_Transaction::f_getTransactionId()	{
	return m_transactionId;
}

void c_Transaction::f_setData(t_Data p_data)	{
	m_data = p_data;
}

t_LineType c_Transaction::f_getLineType()	{
	return m_lineType;
}

void c_Transaction::f_setLineType(t_LineType p_lineType)	{
	m_lineType = p_lineType;
}

void c_Transaction::f_setProbeType(t_ProbeType p_probeType)    {
    m_probeType = p_probeType;
}

t_ProbeType c_Transaction::f_getProbeType()    {
    return m_probeType;
}

void c_Transaction::f_setChangeToXType(t_ChangeToXType p_changeToXType)  {
    m_changeToXType = p_changeToXType;
}

t_ChangeToXType c_Transaction::f_getChangeToXType()  {
    return m_changeToXType;
}

c_NetworkPacket* c_Transaction::f_getNetworkPacket()	{
	return m_networkPacket;
}


bool c_Transaction::f_isFillNoReply()	{
	return m_isFillNoReply;
}
	
void c_Transaction::f_isFillNoReply(bool p_isNoReply)	{
	m_isFillNoReply = p_isNoReply;
}

t_CoreID c_Transaction::f_getNumCores()	{
	return m_numCores;
}

t_Way c_Transaction::f_getReplacedWayPreviousLevel()	{
	return m_replacedWayPreviousLevel;
}

t_Tag c_Transaction::f_getReplacedTagPreviousLevel()	{
	return m_replacedTagPreviousLevel;
}

void c_Transaction::f_setReplacedWayPreviousLevel(t_Way p_replacedWayPreviousLevel)	{
	m_replacedWayPreviousLevel = p_replacedWayPreviousLevel;
}

void c_Transaction::f_setReplacedTagPreviousLevel(t_Way p_replacedTagPreviousLevel)	{
	m_replacedTagPreviousLevel = p_replacedTagPreviousLevel;
}


void c_Transaction::f_convertProtocolRequestToResponse()	{
	switch(m_reqType)	{
	case t_RequestType::E_CORE_READ:
		m_reqType = t_RequestType::E_NUM_RECVQ_TYPE;
		m_responseType = t_ResponseType::E_MISS;
		break;
	case t_RequestType::E_CORE_WRITE:
		m_reqType = t_RequestType::E_NUM_RECVQ_TYPE;
		m_responseType = t_ResponseType::E_VICTIM;
		break;
	case t_RequestType::E_FILL:
		m_reqType = t_RequestType::E_NUM_RECVQ_TYPE;
		m_responseType = t_ResponseType::E_READ_RESP;
		break;
	case t_RequestType::E_PROBE_REQ:
		m_reqType = t_RequestType::E_NUM_RECVQ_TYPE;
		m_responseType = t_ResponseType::E_PROBE_REQ_SEND;
		break;
	case t_RequestType::E_PROBE_RESP:
		m_reqType = t_RequestType::E_NUM_RECVQ_TYPE;
		m_responseType = t_ResponseType::E_PROBE_RESP_SEND;
		break;
	case t_RequestType::E_PREFETCH_RECV:
		m_reqType = t_RequestType::E_NUM_RECVQ_TYPE;
		m_responseType = t_ResponseType::E_PREFETCH_SEND;
		break;
	default:
		break;	
	}
}

void c_Transaction::f_convertProtocolResponseToRequest()	{
	switch(m_responseType)  {
    case t_ResponseType::E_READ_RESP:
        m_reqType = t_RequestType::E_FILL;
        m_responseType = t_ResponseType::E_NUM_SENDQ_TYPE;
        break;
    case t_ResponseType::E_PROBE_REQ_SEND:
        m_reqType = t_RequestType::E_PROBE_REQ;
        m_responseType = t_ResponseType::E_NUM_SENDQ_TYPE;
        break;
    case t_ResponseType::E_PROBE_RESP_SEND:
        m_reqType = t_RequestType::E_PROBE_RESP;
        m_responseType = t_ResponseType::E_NUM_SENDQ_TYPE;
        break;
    case t_ResponseType::E_MISS:
		m_reqType = t_RequestType::E_CORE_READ;
        m_responseType = t_ResponseType::E_NUM_SENDQ_TYPE;
        break;
	case t_ResponseType::E_VICTIM:
		m_reqType = t_RequestType::E_CORE_WRITE;
        m_responseType = t_ResponseType::E_NUM_SENDQ_TYPE;
        break;
    case t_ResponseType::E_PREFETCH_SEND:
        m_reqType = t_RequestType::E_PREFETCH_RECV;
        m_responseType = t_ResponseType::E_NUM_SENDQ_TYPE;
        break;
    }
}
