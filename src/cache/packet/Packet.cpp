#include "Packet.hpp"
#include "Transaction.hpp"
#include "TypeToString.hpp"
#include <assert.h>
#include <iomanip>
#include <iostream>

t_Counter c_Packet::m_packetIdGenerator = 0;

c_Packet::c_Packet()    {
    f_initPacket(0);
}

t_Counter c_Packet::f_getPacketId()   {
    return m_packetId;
}

void c_Packet::f_copyPacket(c_Packet *p_packet)   {
    m_packetId = m_packetIdGenerator;
    m_transaction = NULL;

    m_portNo = p_packet->f_getPortNo();
    m_issuePortNo = p_packet->f_getIssuePort();
	
	m_tag = p_packet->f_getTag();
	m_index = p_packet->f_getIndex();
	m_way = p_packet->f_getWay();
    m_bank = p_packet->f_getBank();
    m_slice = p_packet->f_getSlice();
    m_blockAddress = p_packet->f_getBlockAddress();

	m_blockOffset = p_packet->f_getBlockOffset();

	m_reqCtrlState = E_RCS_IDLE;
	m_mdCtrlState = E_MCS_IDLE;
	
	m_reqTime = p_packet->f_getRequestTime();
	m_decodeTime = p_packet->f_getDecodeTime();
	m_issueTime = p_packet->f_getIssueTime();
    m_storageRespTime = p_packet->f_getStorageRespTime(); 
	m_forwardTime = p_packet->f_getForwardTime();
	m_missResponseTime = p_packet->f_getMissResponseTime(); 
	m_responseTime = p_packet->f_getResponseTime();
	m_evictionTime = p_packet->f_getEvictionTime();
	m_writebackTime = p_packet->f_getWritebackTime();

	m_reserve = false;

	m_drop = false;
	m_requireData = false;
    m_servicedFromSendQ = false;
    m_reflow = p_packet->f_isReflow();
    m_backProbe = p_packet->f_isBackProbe();

	m_lookupStatus = E_LUS_NA;
    m_fillGenerated = false;
    m_victimizerPacket = NULL;
    m_victimizedPacket = NULL;
    m_writeAllocate = p_packet->f_isWriteAllocate();

    m_QCredit = p_packet->f_getQCredit();
    m_SHRCredit = p_packet->f_getSHRCredit();
    m_isHitOtherSource = p_packet->f_hitOtherSource();

	m_packetIdGenerator++;
}

bool c_Packet::f_isBackProbe() {
    return m_backProbe;
}

void c_Packet::f_setBackProbe(bool p_backProbe) {
    m_backProbe = p_backProbe;
}

void c_Packet::f_initPacket(t_PortNo p_portNo)   {
    m_packetId = m_packetIdGenerator;
    m_transaction = NULL;

    m_portNo = p_portNo;
    m_issuePortNo = -1;
	
	m_tag = M_TAG_MAX;
	m_index = M_INDEX_MAX;
    m_bank = M_BANK_MAX;
	m_way = M_WAY_MAX;
    m_blockAddress = M_TAG_MAX;
    m_slice = M_SLICE_MAX;

	m_blockOffset = M_OFFSET_MAX;

	m_reqCtrlState = E_RCS_IDLE;
	m_mdCtrlState = E_MCS_IDLE;
	
	m_reqTime = 0;
	m_decodeTime = 0;
	m_issueTime = 0;
    m_storageRespTime = 0; 
	m_forwardTime = 0;
	m_missResponseTime = 0; 
	m_responseTime = 0;
	m_evictionTime = 0;
	m_writebackTime = 0;

	m_reserve = false;

	m_drop = false;
	m_requireData = false;
    m_isPrefetch = false;
    m_servicedFromSendQ = false;
    m_backProbe = false;
    m_reflow = false;
	m_lookupStatus = E_LUS_NA;

    m_victimizerPacket = NULL;
    m_victimizedPacket = NULL;
    m_writeAllocate = false;
    m_fillGenerated = false;
    m_isHitOtherSource = false;

    m_QCredit = 0;
    m_SHRCredit = 0;

	m_packetIdGenerator++;
}

void c_Packet::f_setTransaction(c_Transaction *p_transaction)   {
    m_transaction = p_transaction;
    if(p_transaction->f_getRequestType() == E_CORE_WRITE)   {
        m_requireData = true;
    }
}

bool c_Packet::f_isFillGenerated()    {
    return m_fillGenerated;
}

void c_Packet::f_fillGenerated()    {
    m_fillGenerated = true;
}	

c_Transaction *c_Packet::f_getTransaction() {
    return m_transaction;
}
	
void c_Packet::f_setSlice(t_Slice p_slice)  {
    m_slice = p_slice;
}
	
void c_Packet::f_setIndex(t_Index p_index)    {
    m_index = p_index;
}
	
void c_Packet::f_setWay(t_Way p_way)    {
    m_way = p_way;
}
	
void c_Packet::f_setTag(t_Tag p_tag)    {
    m_tag = p_tag;
}

void c_Packet::f_setBank(t_Bank p_bank)  {
    m_bank = p_bank;
}

void c_Packet::f_setLookupStatus(t_LookUpStatus p_lookUpStatus)   {
    m_lookupStatus = p_lookUpStatus;
}

void c_Packet::f_setBlockOffset(t_BlockOffset p_blockOffset)  {
    m_blockOffset = p_blockOffset;
}

void c_Packet::f_setBlockAddress(t_BlockAddress p_blockAddress)    {
    m_blockAddress = p_blockAddress;
}

void c_Packet::f_setRequireData(bool p_requireData)   {
    m_requireData = p_requireData;
}

void c_Packet::f_drop()   {
    m_drop = true;
}

void c_Packet::f_victimizerPacket(c_Packet *p_packet)   {
    m_victimizerPacket = p_packet;
}

void c_Packet::f_victimizedPacket(c_Packet *p_packet)   {
    m_victimizedPacket = p_packet;
}

t_Index c_Packet::f_getIndex()    {
    return m_index;
}

t_Bank c_Packet::f_getBank()  {
    return m_bank;
}
	
t_Way c_Packet::f_getWay()    {
    return m_way;
}
	
t_Tag c_Packet::f_getTag()    {
    return m_tag;
}

t_Slice c_Packet::f_getSlice() {
    return m_slice;
}

t_BlockAddress c_Packet::f_getBlockAddress()    {
    return m_blockAddress;
}
	
t_LookUpStatus c_Packet::f_getLookupStatus()  {
    return m_lookupStatus;
}
	
t_BlockOffset c_Packet::f_getBlockOffset()    {
    return m_blockOffset;
}
	
bool c_Packet::f_getRequireData() {
    return m_requireData;
}

bool c_Packet::f_isDropped()  {
    return m_drop;
}	

c_Packet* c_Packet::f_victimizerPacket()    {
    return m_victimizerPacket;
}

c_Packet* c_Packet::f_victimizedPacket()    {
    return m_victimizedPacket;
}

void c_Packet::f_reset()  {
    if(m_QCredit != 0 || m_SHRCredit != 0)  {
        f_logError(NULL, "Invalid Credit Management");
    }
    m_packetId = -1;
    m_transaction = NULL;

    m_portNo = -1;
    m_issuePortNo = -1;
	
	m_tag = M_TAG_MAX;
	m_index = M_INDEX_MAX;
    m_bank = M_BANK_MAX;
	m_way = M_WAY_MAX;
    m_blockAddress = M_TAG_MAX;
    m_slice = M_SLICE_MAX;

	m_blockOffset = M_OFFSET_MAX;

	m_reqCtrlState = E_RCS_IDLE;
	m_mdCtrlState = E_MCS_IDLE;
	
	m_reqTime = 0;
	m_decodeTime = 0;
	m_issueTime = 0;
    m_storageRespTime = 0; 
	m_forwardTime = 0;
	m_missResponseTime = 0; 
	m_responseTime = 0;
	m_evictionTime = 0;
	m_writebackTime = 0;

	m_reserve = false;

	m_drop = false;
	m_requireData = false;
    m_isPrefetch = false;
    m_servicedFromSendQ = false;
    m_backProbe = false;
    m_fillGenerated = false;
    m_reflow = false;
	m_lookupStatus = E_LUS_NA;

    m_victimizerPacket = NULL;
    m_victimizedPacket = NULL;
    m_writeAllocate = false;
    m_isHitOtherSource = false;

    m_QCredit = 0;
    m_SHRCredit = 0;
}

void c_Packet::f_setRequestControllerState(t_RequestControllerState p_state)  {
    m_reqCtrlState = p_state;
}

t_RequestControllerState c_Packet::f_getRequestControllerState()  {
    return m_reqCtrlState;
}

void c_Packet::f_setMetadataControllerState(t_MetadataControllerState p_state)    {
    m_mdCtrlState = p_state;
}

t_MetadataControllerState c_Packet::f_getMetadataControllerState()    {
    return m_mdCtrlState;
}

std::string c_Packet::f_toString(c_Controller *p_ctrl,std::string p_message) {
    std::stringstream m_blockAddressStream;
    std::stringstream m_tagStream;
    if(m_reqCtrlState != t_RequestControllerState::E_RCS_FETCHED) {
        m_blockAddressStream<<std::hex<<m_blockAddress;
        m_tagStream<<std::hex<<m_tag;
    }
    else    {
        m_blockAddressStream<<"NA";
        m_tagStream<<"NA";
    }
    std::stringstream l_pktPtrStrStream;
	if(m_victimizedPacket != NULL)	{
		const void * address = static_cast<const void*>(m_victimizedPacket);
		l_pktPtrStrStream << address;
	} 
	else	{
		l_pktPtrStrStream << "NA";
	}
    if(m_transaction == NULL)   {
        std::cout<<"PacketID:"<<std::to_string(m_packetId)<<std::endl;
        assert(false);
    }  
    return (std::to_string(m_packetId) + "\t" +  m_tagStream.str() 
     + "\t" + std::to_string(m_index) + "\t" + std::to_string(m_bank)
     + "\t" + std::to_string(m_way) + "\t" + std::to_string(m_portNo) + "\t" + std::to_string(m_issuePortNo)
     + "\t" + c_TypeToString::f_requestControllerStateString(m_reqCtrlState) + "\t" + c_TypeToString::f_metadataControllerStateString(m_mdCtrlState)
     + "\t" + c_TypeToString::f_lookUpStateString(m_lookupStatus) + "\t" + (m_writeAllocate ? "Y" : "N") +
     + "\t" + std::to_string(m_reqTime) + "\t" + std::to_string(m_decodeTime) + "\t" + std::to_string(m_issueTime) 
     + "\t" + std::to_string(m_forwardTime) + "\t" + std::to_string(m_missResponseTime) + "\t" + std::to_string(m_responseTime) 
     + "\t" + std::to_string(m_evictionTime) + "\t" + std::to_string(m_writebackTime) + "\t" + (m_drop ? "Y\t" : "N\t") + l_pktPtrStrStream.str() + "\t" + std::to_string(m_QCredit) + "\t" + std::to_string(m_SHRCredit));
}

t_Time c_Packet::f_getRequestTime()   {
    return m_reqTime;
}

t_Time c_Packet::f_getDecodeTime()  {
    return m_decodeTime;
}

t_Time c_Packet::f_getIssueTime()   {
    return m_issueTime;
}

t_Time c_Packet::f_getStorageRespTime() {
    return m_storageRespTime;
}

t_Time c_Packet::f_getForwardTime() {
    return m_forwardTime;
}

t_Time c_Packet::f_getMissResponseTime()    {
    return m_missResponseTime;
}

t_Time c_Packet::f_getResponseTime()    {
    return m_responseTime;
}

t_Time c_Packet::f_getEvictionTime()    {
    return m_evictionTime;
}

t_Time c_Packet::f_getWritebackTime()   {
    return m_writebackTime;
}

bool c_Packet::f_isWriteAllocate()    {
    return m_writeAllocate;
}

void c_Packet::f_setWriteAllocate(bool p_writeAllocate)    {
    m_writeAllocate = p_writeAllocate;
}

bool c_Packet::f_matchIndex(c_Packet *p_packet)   {
    return m_index == p_packet->f_getIndex();
}


void c_Packet::f_reserve(bool p_reserve)  {
    m_reserve = p_reserve;
}

bool c_Packet::f_reserve()  {
    return m_reserve;
}

void c_Packet::f_setPortNo(t_PortNo p_portNo) {
    m_portNo = p_portNo;
}

t_PortNo c_Packet::f_getPortNo()    {
    return m_portNo;
}

void c_Packet::f_setFetchTime(t_Time p_time)  {
    m_reqTime = p_time;
}
	
void c_Packet::f_setDecodeTime(t_Time p_time) {
    m_decodeTime = p_time;
}
	
void c_Packet::f_setIssueTime(t_Time p_time)  {
    m_issueTime = p_time;
}
	
void c_Packet::f_setResponseTime(t_Time p_time)   {
    m_responseTime = p_time;
}

void c_Packet::f_setForwardTime(t_Time p_time)  {
    m_forwardTime = p_time;
}
	
void c_Packet::f_setWritebackTime(t_Time p_time)    {
    m_writebackTime = p_time;
}

t_PortNo c_Packet::f_getIssuePort()    {
    return m_issuePortNo;
}

void c_Packet::f_setIssuePort(t_PortNo p_issuePortNo)    {
    m_issuePortNo = p_issuePortNo;
}

void c_Packet::f_setServicedFromSendQ() {
    m_servicedFromSendQ = true;
}

bool c_Packet::f_getServicedFromSendQ() {
    return m_servicedFromSendQ;
}

void c_Packet::f_setReflow(bool p_reflow) {
    m_reflow = p_reflow;
}

bool c_Packet::f_isReflow()    {
    return m_reflow;
}

void c_Packet::f_releaseQCredit()   {
    m_QCredit--;
    if(m_QCredit < 0)  {
        f_logError(NULL, "Invalid Credit Management");
    }
}
	
void c_Packet::f_reserveQCredit()   {
    m_QCredit++;
    if(m_QCredit >= t_ResponseType::E_NUM_SENDQ_TYPE)  {
        f_logError(NULL, "Invalid Credit Management");
    }
}

void c_Packet::f_releaseSHRCredit() {
    m_SHRCredit--;
    if(m_SHRCredit < 0)  {
        f_logError(NULL, "Invalid Credit Management");
    }
}

void c_Packet::f_reserveSHRCredit() {
    m_SHRCredit++;
    if(m_SHRCredit >= t_SHRType::E_NUM_SHR_TYPE)  {
        f_logError(NULL, "Invalid Credit Management");
    }
}

t_Credit c_Packet::f_getSHRCredit() {
    return m_SHRCredit;
}

t_Credit c_Packet::f_getQCredit() {
    return m_QCredit;
}

void c_Packet::f_transferCredit(c_Packet *p_victimPacket) {
    this->f_releaseQCredit();
    p_victimPacket->f_reserveQCredit();
    this->f_releaseSHRCredit();
    p_victimPacket->f_reserveSHRCredit();
}

void c_Packet::f_hitOtherSource(bool l_isHitOtherSource)  {
    m_isHitOtherSource = l_isHitOtherSource;
}
	
bool c_Packet::f_hitOtherSource()   {
    return m_isHitOtherSource;
}