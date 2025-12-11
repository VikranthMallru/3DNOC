#ifndef REQUEST_H
#define REQUEST_H

#include "CacheLogger.hpp"
#include "types.hpp"


class c_Request : public c_CacheLogger {
private:
    t_Time m_memoryAllocTime;
    bool m_isAllocated;
public:
    c_Request();
    virtual void f_reset() = 0;
    void f_setMemoryAllocTime(t_Time p_allocTime);
    bool f_checkLeak(t_Time p_currTime);
    bool f_isAllocated();
    void f_deAllocate();
};

#endif