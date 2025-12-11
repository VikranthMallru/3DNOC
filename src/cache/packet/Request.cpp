#include "Request.hpp"

c_Request::c_Request()  {
    m_memoryAllocTime = 0;
    m_isAllocated = false;
}

void c_Request::f_setMemoryAllocTime(t_Time p_allocTime)   {
    m_memoryAllocTime = p_allocTime;
    m_isAllocated = true;
}

bool c_Request::f_checkLeak(t_Time p_currTime)   {
    return ((p_currTime - m_memoryAllocTime) > 10000);
}

bool c_Request::f_isAllocated() {
    return m_isAllocated;
}

void c_Request::f_deAllocate()  {
    m_isAllocated = false;
}