#ifndef MASTERPORT_H
#define MASTERPORT_H

#include "types.hpp"
#include "Transaction.hpp"

using namespace std;
class c_Controller;
class c_SlavePort;

class c_MasterPort {
private:
    std::string m_portName;
    c_Controller *m_ctrl;
    c_SlavePort *m_slavePort;
    t_Credit m_portNo;
    t_ResponseType m_respType;
    c_Transaction *m_transaction;
public:
    c_MasterPort(c_Controller *p_ctrl,std::string p_portName, t_Credit p_portNo, t_ResponseType m_respType);
    void f_clockCycle();
    c_Transaction* f_NextTransaction();
    void f_bind(c_SlavePort *p_slavePort);
    void f_portCheck();
    std::string f_toString();
};

#endif