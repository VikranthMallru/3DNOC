
#ifndef REQUESTPOOL_H
#define REQUESTPOOL_H
#include "Request.hpp"
#include "types.hpp"
#include <Queue.hpp>

template <class T>
class c_RequestPool  {
protected:
    static c_Queue *m_list;
    static c_Queue *m_allocList;
    static t_Size m_poolSize;
    static t_Size m_currentAllocSize;
    static t_Size m_currentPoolSize;
public:
    c_RequestPool(t_Credit p_poolSize);
    ~c_RequestPool();
    static void f_checkLeak(t_Time p_currTime);
    static T* f_allocate(t_Time p_time);
    static void f_deAllocate(T* p_request);
};

#endif