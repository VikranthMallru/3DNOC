#include "FixedLatencyStoragePacket.hpp"

c_FixedLatencyStoragePacket::c_FixedLatencyStoragePacket()  {}

void c_FixedLatencyStoragePacket::f_initPacket(c_Transaction *p_transaction, t_FixedLatencyRequestType p_reqType, t_Time p_reqTime)   {
    m_transaction = p_transaction;
    m_reqType = p_reqType;
    m_reqTime = p_reqTime;
    m_isDirectoryFillSent = false;
}

t_FixedLatencyRequestType c_FixedLatencyStoragePacket::f_getReqType()   {
    return m_reqType;
}

t_Time c_FixedLatencyStoragePacket::f_getReqTime()  {
    return m_reqTime;
}

t_Address c_FixedLatencyStoragePacket::f_getStorageAddress()    {
    return m_transaction->f_getPhysicalAddress();
}
    
c_Transaction* c_FixedLatencyStoragePacket::f_getTransaction()  {
    return m_transaction;
}
    
void c_FixedLatencyStoragePacket::f_reset()  {
    m_transaction = NULL;
    m_reqType = t_FixedLatencyRequestType::E_NUM_FIXED_LATENCY_REQTYPE;
    m_reqTime = 0;
}

std::string c_FixedLatencyStoragePacket::f_toString(c_Controller *p_ctrl,std::string p_message)  {
    return "";
}

void c_FixedLatencyStoragePacket::f_setRequestType(t_FixedLatencyRequestType p_reqType) {
    m_reqType = p_reqType;
}

bool c_FixedLatencyStoragePacket::f_isDirectoryFillSent()   {
    return m_isDirectoryFillSent;
}
    
void c_FixedLatencyStoragePacket::f_directoryFillSent(bool p_isSent)    {
    m_isDirectoryFillSent = p_isSent;
}
    