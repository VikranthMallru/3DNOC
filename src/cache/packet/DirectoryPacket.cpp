#include "DirectoryPacket.hpp"
#include "Transaction.hpp"
#include <sstream>
#include "TypeToString.hpp"

t_Counter c_DirectoryPacket::m_packetIdGenerator = 0;

c_DirectoryPacket::c_DirectoryPacket()  {}

c_DirectoryPacket::~c_DirectoryPacket()  {
    f_reset();
}

t_Counter c_DirectoryPacket::f_getPacketId()    {
    return m_packetId;
}

void c_DirectoryPacket::f_initPacket(t_Size p_numSources) {
    m_blockAddress = M_TAG_MAX;
    m_transaction = NULL;
    m_numSources = p_numSources;
    m_lookUpStatus = new t_LookUpStatus[m_numSources];
    for(t_Size l_sourceId = 0; l_sourceId < m_numSources; l_sourceId++) {
        m_lookUpStatus[l_sourceId] = E_LUS_NA;
    }
    m_tag = M_TAG_MAX;
    m_index = M_INDEX_MAX;
    m_way = M_WAY_MAX;
    m_slice = M_SLICE_MAX;
    m_sourceId = M_CORE_MAX;
    m_sourceToProbe = M_CORE_MAX;
    m_packetId = m_packetIdGenerator;
    m_packetIdGenerator++;
}

void c_DirectoryPacket::f_copyPacket(c_DirectoryPacket *p_packet)   {
    m_blockAddress = p_packet->f_getBlockAddress();
    m_transaction = NULL;
    m_lookUpStatus = new t_LookUpStatus[m_numSources];
    for(t_Size l_sourceId = 0; l_sourceId < m_numSources; l_sourceId++) {
        m_lookUpStatus[l_sourceId] = p_packet->f_getLookupStatus(l_sourceId);
    }
    m_tag = p_packet->f_getTag();
    m_way = p_packet->f_getWay();
    m_index = p_packet->f_getIndex();
    m_slice = p_packet->f_getSlice();
    m_sourceId = p_packet->f_getReqSource();
    m_sourceToProbe = p_packet->f_getSourceToProbe();
    m_packetId = m_packetIdGenerator;
    m_packetIdGenerator++;
}


void c_DirectoryPacket::f_setSlice(t_Slice p_slice) {
    m_slice = p_slice;
}

void c_DirectoryPacket::f_setIndex(t_Index p_index) {
    m_index = p_index;
}

void c_DirectoryPacket::f_setWay(t_Way p_way)   {
    m_way = p_way;
}

void c_DirectoryPacket::f_setTag(t_Tag p_tag)   {
    m_tag = p_tag;
}

void c_DirectoryPacket::f_setReqSource(t_CoreID p_coreId, t_LineType p_lineType, t_CacheLevel p_cacheLevel) {
    if(p_cacheLevel == 2)   {
        m_sourceId = p_coreId;
    }
    else if(p_cacheLevel == 1)   {
        if(p_lineType == E_DATA)    {
            m_sourceId = 1;
        }
        else {
            m_sourceId = 0;
        }
    }
}

void c_DirectoryPacket::f_setLookupStatus(t_LookUpStatus p_lookUpStatus, t_Size p_sourceId)    {
    m_lookUpStatus[p_sourceId] = p_lookUpStatus;
}

void c_DirectoryPacket::f_setBlockAddress(t_BlockAddress p_blockAddress)    {
    m_blockAddress = p_blockAddress;
}

void c_DirectoryPacket::f_reset()   {
    m_packetId = -1;
    m_blockAddress = M_TAG_MAX;
    m_transaction = NULL;
    for(t_Size l_sourceId = 0; l_sourceId < m_numSources; l_sourceId++) {
        m_lookUpStatus[l_sourceId] = E_LUS_NA;
    }
    delete m_lookUpStatus;
    m_sourceToProbe = M_CORE_MAX;
    m_tag = M_TAG_MAX;
    m_index = M_INDEX_MAX;
    m_way = M_WAY_MAX;
    m_slice = M_SLICE_MAX;
    m_sourceId = M_CORE_MAX;
}

t_Index c_DirectoryPacket::f_getIndex() {
    return m_index;
}

t_Way c_DirectoryPacket::f_getWay() {
    return m_way;
}

t_Tag c_DirectoryPacket::f_getTag() {
    return m_tag;
}

t_Slice c_DirectoryPacket::f_getSlice() {
    return m_slice;
}

t_BlockAddress c_DirectoryPacket::f_getBlockAddress()   {
    return m_blockAddress;
}

t_LookUpStatus c_DirectoryPacket::f_getLookupStatus(t_Size p_sourceId)   {
    return m_lookUpStatus[p_sourceId];
}

t_CoreID c_DirectoryPacket::f_getReqSource() {
    return m_sourceId;
}

c_Transaction *c_DirectoryPacket::f_getTransaction()   {
    return m_transaction;
}
    
void c_DirectoryPacket::f_setTransaction(c_Transaction *p_transaction) {
    m_transaction = p_transaction;
}

t_CoreID c_DirectoryPacket::f_getSourceToProbe()    {
    return m_sourceToProbe;
}

void c_DirectoryPacket::f_setSourceToProbe(t_CoreID p_sourceToProbe)   {
    m_sourceToProbe = p_sourceToProbe;
}
	

std::string c_DirectoryPacket::f_toString(c_Controller *p_ctrl,std::string p_message)  {
    std::stringstream m_blockAddressStream;
    std::stringstream m_tagStream;
    std::stringstream m_lus;
    m_blockAddressStream<<std::hex<<m_blockAddress;
    m_tagStream<<std::hex<<m_tag; 
    for(t_Size l_sourceId = 0; l_sourceId < m_numSources; l_sourceId++) {
        m_lus<<c_TypeToString::f_lookUpStateString(m_lookUpStatus[l_sourceId]);
        if(l_sourceId != m_numSources - 1)  {
            m_lus<<",";
        }
    }
    return m_blockAddressStream.str() + "\t" + std::to_string(m_packetId) + "\t" +  m_tagStream.str() 
     + "\t" + std::to_string(m_index) + "\t" + std::to_string(m_way)
     + "\t" + m_lus.str();
}