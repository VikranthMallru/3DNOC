#ifndef SLAVEPORT_H
#define SLAVEPORT_H

#include "types.hpp"
#include <string>

class c_Controller;
class c_Transaction;
class c_MasterPort;

class c_SlavePort {
private:
    c_Controller *m_ctrl;
    std::string m_portName;
    t_Credit m_portNo;
    c_MasterPort *m_masterPort;
    t_RequestType m_requestType;
public:
    c_SlavePort(c_Controller *p_ctrl, std::string p_name, t_Credit p_portNo, t_RequestType p_requestType);
    void f_clockCycle();
    void f_bind(c_MasterPort *p_masterPort);
    void f_portCheck();
    std::string f_toString();
};

#endif