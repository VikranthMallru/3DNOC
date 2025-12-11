#include "Request.hpp"
#include "Transaction.hpp"
#include "types.hpp"
#ifndef FIXEDLATENCYSTORAGEPACKET_H
#define FIXEDLATENCYSTORAGEPACKET_H
class c_FixedLatencyStoragePacket : public c_Request	{
private:
	c_Transaction *m_transaction;
    t_FixedLatencyRequestType m_reqType;
    t_Time m_reqTime;
    t_Address m_address;
    bool m_isDirectoryFillSent;
public:
    c_FixedLatencyStoragePacket();
    void f_initPacket(c_Transaction *p_transaction, t_FixedLatencyRequestType p_reqType, t_Time p_reqTime);
    t_FixedLatencyRequestType f_getReqType();
    void f_setRequestType(t_FixedLatencyRequestType p_reqType);
    t_Time f_getReqTime();
    t_Address f_getStorageAddress();
    c_Transaction* f_getTransaction();
    bool f_isDirectoryFillSent();
    void f_directoryFillSent(bool p_isSent);
    void f_reset();
    std::string f_toString(c_Controller *p_ctrl,std::string p_message);
};
#endif