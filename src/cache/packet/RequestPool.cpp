#include "RequestPool.hpp"
#include <assert.h>
#include <iostream>
#include "Transaction.hpp"
#include "Packet.hpp"
#include "NetworkPacket.hpp"
#include "DirectoryPacket.hpp"
#include "FixedLatencyStoragePacket.hpp"
#include "Queue.hpp"


template <class T>
c_Queue* c_RequestPool<T>::m_list = NULL;

template <class T>
c_Queue* c_RequestPool<T>::m_allocList = NULL;

template <class T>
t_Credit c_RequestPool<T>::m_poolSize = 0;

template <class T>
t_Credit c_RequestPool<T>::m_currentAllocSize = 0;

template <class T>
t_Credit c_RequestPool<T>::m_currentPoolSize = 0;


template <class T>
c_RequestPool<T>::c_RequestPool(t_Credit p_poolSize) {
    m_list = new c_Queue("CompletePool",t_RequestType::E_NUM_RECVQ_TYPE, t_ResponseType::E_NUM_SENDQ_TYPE,0,p_poolSize, t_QueueType::E_NUM_Q_TYPES);
    for(t_Credit l_index = 0; l_index < p_poolSize; l_index++)   {
        m_list->f_allocate(new T());
    }
    m_poolSize = p_poolSize;
    m_allocList = new c_Queue("AllPool",t_RequestType::E_NUM_RECVQ_TYPE, t_ResponseType::E_NUM_SENDQ_TYPE,0,p_poolSize, t_QueueType::E_NUM_Q_TYPES);
}

template <class T>
c_RequestPool<T>::~c_RequestPool()  {
    m_list->f_clear();
}

template <class T>
T* c_RequestPool<T>::f_allocate(t_Time p_currTime) {
    if(!m_list->f_empty())  {
        T *l_req = (T*)m_list->f_get(0);
        ((c_Request*)l_req)->f_setMemoryAllocTime(p_currTime);
        m_list->f_deallocate(l_req);
        m_allocList->f_allocate(l_req);
        f_checkLeak(p_currTime);
        return l_req;
    }
    else    {
        std::cout<<"Request Pool Empty"<<std::endl;
        assert(false);
    }
}

template <class T>
void c_RequestPool<T>::f_deAllocate(T *p_request) {
    m_list->f_allocate(p_request);
    m_allocList->f_deallocate(p_request);
    p_request->f_reset();
}

template <class T>
void c_RequestPool<T>::f_checkLeak(t_Time p_currTime) {
    for(t_Credit l_index = 0; l_index < m_allocList->f_size(); l_index++)   {
        T *l_req = (T*) m_allocList->f_get(l_index);
        if(((c_Request*)l_req)->f_checkLeak(p_currTime))    {
            l_req->f_logError(NULL, "Definite Leak");
        }
    }
}


template class c_RequestPool<c_Transaction>;
template class c_RequestPool<c_FixedLatencyStoragePacket>;
template class c_RequestPool<c_Packet>;
template class c_RequestPool<c_NetworkPacket>;
template class c_RequestPool<c_DirectoryPacket>;
