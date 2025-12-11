#include "MasterPort.hpp"
#include "SlavePort.hpp"
#include "Controller.hpp"
#include <stdlib.h> 
#include <time.h>
#include <sstream>
#include <assert.h>
#include "TypeToString.hpp"

c_MasterPort::c_MasterPort(c_Controller *p_ctrl, std::string p_portName, t_Credit p_portNo,
	t_ResponseType p_respType) {
	m_portNo = p_portNo;
	m_ctrl = p_ctrl;
	m_portName = p_portName;
    m_respType = p_respType;
	m_transaction = NULL;
}

void c_MasterPort::f_bind(c_SlavePort *p_slavePort)	{
	m_slavePort = p_slavePort;
}

void c_MasterPort::f_clockCycle()  {
	if(m_transaction == NULL)	{
		m_transaction = m_ctrl->f_retireTransaction(m_portNo,m_respType);
	}
}

c_Transaction *c_MasterPort::f_NextTransaction()	{
	c_Transaction *l_transaction = m_transaction;
	m_transaction = NULL;
	return l_transaction;
}

void c_MasterPort::f_portCheck()	{
	assert(m_slavePort != NULL);
}

std::string c_MasterPort::f_toString()    {
    return "" + m_ctrl->f_getName() + " PN:" + m_portName + " PNo: " + to_string(m_portNo) + " ReqT: " + c_TypeToString::f_responseTypeString(m_respType); 
}