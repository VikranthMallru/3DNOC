#ifndef QUEUE_H
#define QUEUE_H

#include "types.hpp"
#include "Transaction.hpp"
#include "Packet.hpp"
#include <string>
#include <queue>
#include <vector>


class c_Queue   {
private:
    std::vector<c_Request*> m_queue;
    t_Credit m_currentSize;
    t_Credit m_maxSize;
    t_Credit m_reservedCredit;
    t_QueueType m_queueType;
    t_PortNo m_portNo;
    t_RequestType m_reqType;
    t_ResponseType m_respType;
    std::string m_name;
public:
    c_Queue(std::string p_name,t_RequestType p_reqType, t_ResponseType p_respType,t_PortNo p_portNo,t_Credit p_maxSize, t_QueueType p_queueType);
    c_Request* f_conflictCheck(c_Request *p_packet);
    void f_erase(c_Request *p_packet);
    t_QueueType f_getQueueType();
    c_Request *f_findNextBypassRequest();
    void f_deallocate(c_Request *p_packet);
    bool f_hasCredit();
    t_Credit f_availableCredit();
    void f_allocate(c_Request *p_packet);
    void f_findNextRequest(c_PacketQueueLocation *l_location, t_Bank p_bank);
    c_Request *f_get(t_Credit p_index);
    t_Credit f_size();
    bool f_empty();
    void f_reserveCredit(c_Packet *p_packet);
    void f_releaseCredit(c_Packet *p_packet);
    t_Credit f_getMaxSize();
    void f_display();
    std::string f_getName();
    void f_clear();
};

#endif