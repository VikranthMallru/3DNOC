#include "SlavePort.hpp"
#include "Transaction.hpp"
#include "Controller.hpp"
#include "MasterPort.hpp"
#include "TypeToString.hpp"
#include <assert.h>

using namespace std;

c_SlavePort::c_SlavePort(c_Controller *p_ctrl, std::string p_name, 
    t_Credit p_portNo, t_RequestType p_requestType) {
    m_ctrl = p_ctrl;
    m_portName = p_name;
    m_portNo = p_portNo;
    m_requestType = p_requestType;
}

void c_SlavePort::f_bind(c_MasterPort *p_masterPort)  {
    m_masterPort = p_masterPort;
}

void c_SlavePort::f_clockCycle()   {
    if(m_ctrl->f_hasCreditRecvQueue(m_portNo,m_requestType))  {
        c_Transaction* l_transaction = m_masterPort->f_NextTransaction();
        if(l_transaction != NULL)   {
            l_transaction->f_convertProtocolResponseToRequest();
            m_ctrl->f_fetchTransaction(m_portNo,l_transaction);
        }
    }    
}

void c_SlavePort::f_portCheck(){
    if(m_masterPort == NULL)    {
        std::cout<<f_toString()<<std::endl;
        assert(false);
    }
}

std::string c_SlavePort::f_toString()    {
    return "" + m_ctrl->f_getName() + " PN:" + m_portName + " PNo: " + to_string(m_portNo) + " ReqT: " + c_TypeToString::f_requestTypeString(m_requestType); 
}
