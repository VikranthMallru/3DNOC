#include "Queue.hpp"
#include <assert.h>
#include <iostream>
#include <typeinfo>
#include "TypeToString.hpp"

c_Queue::c_Queue(std::string p_name, t_RequestType p_reqType, t_ResponseType p_respType,t_PortNo p_portNo,
        t_Credit p_maxSize, t_QueueType p_queueType)    {
    m_name = p_name;
    m_maxSize = p_maxSize;
    m_queueType = p_queueType;
    m_reqType = p_reqType;
    m_respType = p_respType;
    m_reservedCredit = 0;
    m_currentSize = 0;
    m_portNo = p_portNo;
}

t_Credit c_Queue::f_getMaxSize() {
    return m_maxSize;
}
    
void c_Queue::f_reserveCredit(c_Packet *p_packet) {
    m_reservedCredit++;
    p_packet->f_reserveQCredit();
    if(m_reservedCredit < 0 || m_reservedCredit + m_currentSize > m_maxSize)   {
        std::cout<<"Queue : "<<m_name<<" RESPTYPE: "<<c_TypeToString::f_responseTypeString(m_respType)
            <<" REQTYPE: "<<c_TypeToString::f_requestTypeString(m_reqType)<<
            " QUEUETYPE: "<<c_TypeToString::f_queueTypeString(m_queueType)<<
            " Port: "<<m_portNo<<
            " Credit issues: CURR SIZE: "<< m_currentSize << " RESERVED: "<<m_reservedCredit
            <<"\tMAX SIZE: "<<m_maxSize<<std::endl;
        assert(false);
    }
}
    
void c_Queue::f_releaseCredit(c_Packet *p_packet) {
    m_reservedCredit--;
    p_packet->f_releaseQCredit();
    if(m_reservedCredit < 0 || m_reservedCredit + m_currentSize > m_maxSize)   {
        std::cout<<"Queue : "<<m_name<<" RESPTYPE: "<<c_TypeToString::f_responseTypeString(m_respType)
            <<" REQTYPE: "<<c_TypeToString::f_requestTypeString(m_reqType)<<
            " QUEUETYPE: "<<c_TypeToString::f_queueTypeString(m_queueType)<<
            " Port: "<<m_portNo<<
            " Credit issues: CURR SIZE: "<< m_currentSize << " RESERVED: "<<m_reservedCredit
            <<" \tMAX SIZE: "<<m_maxSize<<std::endl;
        assert(false);
    }
}

c_Request* c_Queue::f_conflictCheck(c_Request *p_packet)  {
    assert(typeid(p_packet) == typeid(c_Packet*));
    for(t_Credit l_packetIndex = 0; l_packetIndex < m_currentSize; l_packetIndex++) {
        if(((c_Packet*) m_queue[l_packetIndex])->f_getBlockAddress() == ((c_Packet*) p_packet)->f_getBlockAddress())    {
            return m_queue[l_packetIndex];
        }
    }
    return NULL;
}

t_QueueType c_Queue::f_getQueueType()    {
    return m_queueType;
}

void c_Queue::f_erase(c_Request *p_packet) {
    for(t_Credit l_packetIndex = 0; l_packetIndex < m_currentSize; l_packetIndex++) {
        if(m_queue[l_packetIndex] == p_packet)    {
            m_queue.erase(m_queue.begin() + l_packetIndex);
            m_currentSize--;
            if(m_currentSize < 0)    {
                std::cout<<"Queue : "<<m_name<<" RESPTYPE: "<<c_TypeToString::f_responseTypeString(m_respType)
                <<" REQTYPE: "<<c_TypeToString::f_requestTypeString(m_reqType)<<
                " QUEUETYPE: "<<c_TypeToString::f_queueTypeString(m_queueType)<<
                " Port: "<<m_portNo<<
                " Credit issues: CURR SIZE: "<< m_currentSize << " RESERVED: "<<m_reservedCredit
                <<" \tMAX SIZE: "<<m_maxSize<<std::endl;
                p_packet->f_logError(NULL,"Causing underflow");
            }
            return;
        }
    }
}

void c_Queue::f_deallocate(c_Request *p_packet)  {
    f_erase(p_packet);
}

bool c_Queue::f_hasCredit() {
    return (m_currentSize + m_reservedCredit) < m_maxSize;
}

t_Credit c_Queue::f_availableCredit()    {
    return m_maxSize - (m_currentSize + m_reservedCredit);
}

void c_Queue::f_allocate(c_Request *p_packet)    {
    m_queue.push_back(p_packet);
    m_currentSize++;
    if(m_currentSize + m_reservedCredit > m_maxSize)    {
        std::cout<<"Queue : "<<m_name<<" RESPTYPE: "<<c_TypeToString::f_responseTypeString(m_respType)
                <<" REQTYPE: "<<c_TypeToString::f_requestTypeString(m_reqType)<<
                " QUEUETYPE: "<<c_TypeToString::f_queueTypeString(m_queueType)<<
                " Port: "<<m_portNo<<
                " Credit issues: CURR SIZE: "<< m_currentSize << " RESERVED: "<<m_reservedCredit
                <<" \tMAX SIZE: "<<m_maxSize<<std::endl;
                
        p_packet->f_logError(NULL,"Causing overflow");
    }
}

void c_Queue::f_findNextRequest(c_PacketQueueLocation *l_location, t_Bank p_bank) {
    for(t_Credit l_packetIndex = 0; l_packetIndex < m_currentSize; l_packetIndex++) {
        c_Packet *l_packet = (c_Packet*) m_queue[l_packetIndex];
        c_Transaction *l_transaction = l_packet->f_getTransaction();
        if(l_transaction->f_getRequestType() == l_location->m_requestType
            && p_bank == l_packet->f_getBank())  {
			l_location->m_packetIndex = l_packetIndex;
            return;
        }
    }
}

void c_Queue::f_display() {
    for(t_Credit l_packetIndex = 0; l_packetIndex < m_currentSize; l_packetIndex++) {
        c_Packet *l_packet = (c_Packet*) m_queue[l_packetIndex];
        if(l_packet->f_getTransaction()->f_getTransactionId() == 276)   {
            l_packet->f_logRelease(NULL,"Queue Entry" + std::to_string(l_packetIndex));
        }
    }
}

c_Request *c_Queue::f_get(t_Credit p_index)  {
    if(p_index >= m_currentSize)    {
        return NULL;
    }
    else    {
        return m_queue[p_index];
    }
}

t_Credit c_Queue::f_size()  {
    return m_currentSize;
}

bool c_Queue::f_empty()  {
    return (m_currentSize == 0);
}

std::string c_Queue::f_getName() {
    return m_name;
}

void c_Queue::f_clear() {
    for(t_Credit l_packetIndex = 0; l_packetIndex < m_currentSize; l_packetIndex++) {
        c_Request *l_request = m_queue[l_packetIndex];
        m_queue.erase(m_queue.begin() + l_packetIndex);
        delete l_request;
        m_currentSize--;
    }
}