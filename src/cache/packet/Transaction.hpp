#include "types.hpp"
#include "Request.hpp"
#include "CacheLogger.hpp"

#ifndef TRANSACTION_H
#define TRANSACTION_H
class c_Packet;
class c_NetworkPacket;
class c_DirectoryPacket;

class c_Transaction : public c_Request	{
private:
	bool m_isFillNoReply;
	t_PhysicalAddress m_phyAddr;
	t_VirtualAddress m_virtAddr;
	t_CacheState m_reqCacheState;
	t_CacheState m_respCacheState;
	t_Size m_dataSize;
	t_Slice m_slice;
	t_Stack m_stack;
	t_RequestType m_reqType;
	t_ResponseType m_responseType;
	t_ProbeType m_probeType;
	t_LineType m_lineType;
	t_ChangeToXType m_changeToXType;
	t_Data m_data;
	bool m_isPrefetch;
	c_Packet* m_cachePacket;
	c_NetworkPacket *m_networkPacket;
	c_DirectoryPacket *m_directoryPacket;
	
	t_CacheLevel m_currentLevel;
	t_CacheLevel m_installLevel;
	t_CacheLevel m_sourceLevel;
	t_Counter m_transactionId;
	t_CoreID m_requestingCore;
	bool *m_probeDestinations;
	t_CoreID m_probeDataSource;
	t_Time m_initTime;
	t_CoreID m_numCores;
	t_Way m_replacedWayPreviousLevel;
	t_Tag m_replacedTagPreviousLevel;
	static t_Counter m_transactionIdGenerator;
public:
	c_Transaction();
	void f_initTransaction(t_CoreID p_numCores,t_VirtualAddress p_virtAddr,t_RequestType p_reqType,t_Data p_data, t_ResponseType p_responseType,
		t_ProbeType p_probeType,t_CacheLevel p_installLevel, t_CacheLevel p_currentLevel, t_CacheState p_reqCacheState, t_Time p_initCycle, t_CoreID p_coreId);
	void f_copyTransaction(c_Transaction *p_transaction);
	~c_Transaction();
	bool f_isPrefetch();
	t_Time f_getInitTime();
	t_Data f_getData();
	t_Size f_getDataSize();
	t_CacheLevel f_getCurrentLevel();
	t_CacheLevel f_getInstallLevel();
	t_ProbeType f_getProbeType();
	t_RequestType f_getRequestType();
	t_ResponseType f_getResponseType();
	t_LineType f_getLineType();
	t_ChangeToXType f_getChangeToXType();
	t_CoreID f_getRequestingCore();
	t_CoreID f_getNumCores();

	t_CacheState f_getRequestCacheState();
	t_CacheState f_getResponseCacheState();
	void f_isPrefetch(bool p_isPrefetch);
	bool f_isFillNoReply();
	void f_isFillNoReply(bool p_isNoReply);
	void f_setProbeDestination(t_Size p_probeDestination);
	bool f_isProbeDestination(t_Size p_probeDestination);
	void f_addPacket(c_Packet *p_packet);
	void f_addNetworkPacket(c_NetworkPacket *p_packet);
	void f_addDirectoryPacket(c_DirectoryPacket* p_directoryPacket);
	c_Packet* f_getPacket();
	c_NetworkPacket* f_getNetworkPacket();
	c_DirectoryPacket* f_getDirectoryPacket();
	void f_removePacket(c_Packet *p_packet);
	void f_removeNetworkPacket(c_NetworkPacket *p_networkPacket);
	void f_removeDirectoryPacket(c_DirectoryPacket *p_directoryPacket);
	void f_locate(t_Stack p_stack, t_Slice p_slice);
	void f_setRequestCacheState(t_CacheState p_state);
	void f_setResponseCacheState(t_CacheState p_state);
	void f_reset();
	void f_setRequestType(t_RequestType p_requestType);	
	void f_setResponseType(t_ResponseType p_responseType);
	void f_setProbeType(t_ProbeType p_probeType);
	void f_setData(t_Data p_data);
	void f_setLineType(t_LineType p_lineType);
	void f_setChangeToXType(t_ChangeToXType p_changeToXType);
	void f_setRequestingCore(t_CoreID p_coreId);
	void f_setCurrentLevel(t_CacheLevel p_cacheLevel);
	t_PhysicalAddress f_getPhysicalAddress();
	t_VirtualAddress f_getVirtualAddress();
	void f_setPhysicalAddress(t_PhysicalAddress p_phyAddr);
	void f_setVirtualAddress(t_VirtualAddress p_virtAddr);

	t_Way f_getReplacedWayPreviousLevel();
	t_Tag f_getReplacedTagPreviousLevel();
	void f_setReplacedTagPreviousLevel(t_Way p_replacedTagPreviousLevel);
	void f_setReplacedWayPreviousLevel(t_Way p_replacedWayPreviousLevel);

	t_Counter f_getTransactionId();	

	std::string f_toString(c_Controller *p_ctrl,std::string p_message);
	void f_convertProtocolResponseToRequest();
	void f_convertProtocolRequestToResponse();
	void f_setProbeDataSource(t_CoreID p_dataSource);
	t_CoreID f_getProbeDataSource();
};

#endif