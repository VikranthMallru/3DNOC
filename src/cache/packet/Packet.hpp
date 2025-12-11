#include "types.hpp"
#include "Request.hpp"

#ifndef PACKET_H
#define PACKET_H
class c_Transaction;
class c_Packet : public c_Request	{
private:
	c_Transaction *m_transaction;
	
	t_PortNo m_portNo;
	t_PortNo m_issuePortNo;

	t_Counter m_packetId;

	t_Tag m_tag;
	t_Index m_index;
	t_Bank m_bank;
	t_Way m_way;
	t_BlockAddress m_blockAddress;

	t_Slice m_slice;

	t_BlockOffset m_blockOffset;
		
	//t_Bank m_bank;
		
	t_RequestControllerState m_reqCtrlState;
	t_MetadataControllerState m_mdCtrlState;
	
	t_Credit m_QCredit;
	t_Credit m_SHRCredit;
	
	t_Time m_reqTime;
	t_Time m_decodeTime;
	t_Time m_issueTime;
	t_Time m_storageRespTime; 
	t_Time m_forwardTime;
	t_Time m_missResponseTime; 
	t_Time m_responseTime;
	t_Time m_evictionTime;
	t_Time m_writebackTime;

	bool m_drop;
	bool m_requireData;
	bool m_isPrefetch;
	bool m_reserve;
	bool m_servicedFromSendQ;
	bool m_writeAllocate;
	bool m_backProbe;
	bool m_fillGenerated;
	bool m_byPassMetadataAccess;
	bool m_reflow;
	bool m_isHitOtherSource;

	t_LookUpStatus m_lookupStatus;
	c_Packet *m_victimizedPacket;
	c_Packet *m_victimizerPacket;
	static t_Counter m_packetIdGenerator;

public:
	c_Packet();
	t_Counter f_getPacketId();
	void f_initPacket(t_PortNo p_portNo);
	void f_copyPacket(c_Packet *p_packet);
	void f_setTransaction(c_Transaction *p_transaction);
	c_Transaction *f_getTransaction();

	void f_setPortNo(t_PortNo p_portNo);
	void f_setSlice(t_Slice p_slice);
	void f_setIndex(t_Index p_index);
	void f_setBank(t_Bank p_bank);
	void f_setWay(t_Way p_way);
	void f_setTag(t_Tag p_tag);
	void f_setLookupStatus(t_LookUpStatus p_lookUpStatus);
	void f_setBlockAddress(t_BlockAddress p_blockAddress);
	void f_setBlockOffset(t_BlockOffset p_blockOffset);
	void f_setRequireData(bool p_requireData);
	void f_drop();	
	void f_reset();
	void f_reserve(bool p_reserve);
	void f_setRequestControllerState(t_RequestControllerState p_state);	
	void f_setMetadataControllerState(t_MetadataControllerState p_state);	
	void f_setFetchTime(t_Time p_time);
	void f_setDecodeTime(t_Time p_time);
	void f_setIssueTime(t_Time p_time);
	void f_setResponseTime(t_Time p_time);
	void f_setForwardTime(t_Time p_time);
	void f_setWritebackTime(t_Time p_time);
	void f_setIssuePort(t_PortNo p_issuePort);
	void f_setServicedFromSendQ();
	void f_victimizerPacket(c_Packet *p_packet);
	void f_victimizedPacket(c_Packet *p_packet);
	void f_setBackProbe(bool p_backProbe);
	void f_setReflow(bool p_reflow);

	void f_setWriteAllocate(bool p_writeAllocate);
	void f_fillGenerated();

	bool f_matchIndex(c_Packet *p_packet);
	bool f_isBackProbe();
	bool f_isFillGenerated();

	t_PortNo f_getIssuePort();
	t_PortNo f_getPortNo();
	t_Index f_getIndex();
	t_Bank f_getBank();
	t_Way f_getWay();
	t_Tag f_getTag();
	t_Slice f_getSlice();
	t_BlockAddress f_getBlockAddress();
	t_LookUpStatus f_getLookupStatus();
	t_BlockOffset f_getBlockOffset();
	bool f_getServicedFromSendQ();
	bool f_reserve();
	bool f_getRequireData();
	bool f_isDropped();	
	bool f_isWriteAllocate();
	bool f_isReflow();
	
	t_Time f_getRequestTime();
	t_Time f_getDecodeTime();
	t_Time f_getIssueTime();
	t_Time f_getStorageRespTime(); 
	t_Time f_getForwardTime();
	t_Time f_getMissResponseTime(); 
	t_Time f_getResponseTime();
	t_Time f_getEvictionTime();
	t_Time f_getWritebackTime();

	c_Packet* f_victimizerPacket();
	c_Packet* f_victimizedPacket();
	t_RequestControllerState f_getRequestControllerState();
	t_MetadataControllerState f_getMetadataControllerState();

	void f_releaseQCredit();
	void f_reserveQCredit();
	void f_releaseSHRCredit();
	void f_reserveSHRCredit();
	t_Credit f_getQCredit();
	t_Credit f_getSHRCredit();
	void f_transferCredit(c_Packet *p_victimPacket);
	void f_hitOtherSource(bool l_isHitOtherSource);
	bool f_hitOtherSource();
	
	std::string f_toString(c_Controller *p_ctrl,std::string p_message);
};
#endif