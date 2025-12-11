#include "types.hpp"
#include "Request.hpp"

#ifndef DIRECTORYPACKET_H
#define DIRECTORYPACKET_H
class c_Transaction;
class c_DirectoryPacket : public c_Request	{
private:
	c_Transaction *m_transaction;
	t_Counter m_packetId;

	t_Tag m_tag;
	t_Index m_index;
	t_Way m_way;
	t_BlockAddress m_blockAddress;
    t_Slice m_slice;
	t_CoreID m_sourceId;
	t_CoreID m_sourceToProbe;
	t_Size m_numSources;

	t_LookUpStatus *m_lookUpStatus;
	static t_Counter m_packetIdGenerator;

public:
	c_DirectoryPacket();
	~c_DirectoryPacket();
	t_Counter f_getPacketId();
	void f_initPacket(t_Size p_numSources);
	void f_copyPacket(c_DirectoryPacket *p_packet);
	
	void f_setSlice(t_Slice p_slice);
	void f_setIndex(t_Index p_index);
	void f_setWay(t_Way p_way);
	void f_setTag(t_Tag p_tag);
	void f_setReqSource(t_CoreID p_coreId, t_LineType p_lineType, t_CacheLevel p_cacheLevel);
	void f_setLookupStatus(t_LookUpStatus p_lookUpStatus,t_Size p_numSources);
	void f_setBlockAddress(t_BlockAddress p_blockAddress);
	void f_reset();
	
	t_Index f_getIndex();
	t_Bank f_getBank();
	t_Way f_getWay();
	t_Tag f_getTag();
    t_Slice f_getSlice();
	t_CoreID f_getReqSource();
	t_BlockAddress f_getBlockAddress();
	t_LookUpStatus f_getLookupStatus(t_Size p_numSources);
	t_BlockOffset f_getBlockOffset();

	t_CoreID f_getSourceToProbe();
	void f_setSourceToProbe(t_CoreID p_sourceToProbe);
	
	c_Transaction *f_getTransaction();
    void f_setTransaction(c_Transaction *p_transaction);
	
	std::string f_toString(c_Controller *p_ctrl,std::string p_message);
};
#endif