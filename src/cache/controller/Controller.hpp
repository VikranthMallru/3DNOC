#include "types.hpp"
#include <string>

#ifndef CONTROLLER_H
#define CONTROLLER_H
class c_CacheController;
class c_CacheConfig;
class c_Transaction;

class c_Controller	{
protected:
	std::string m_name;
	c_Controller *m_ctrl;
	c_CacheConfig *m_config;
	t_CacheLevel m_currentLevel;
public:
	c_Controller(std::string p_name);
	std::string f_getName();
	virtual void f_clockCycle() = 0;
	virtual t_Time f_currentCycle() = 0;
	virtual void f_resetClock() = 0;
	virtual void f_fetch() = 0;
	virtual void f_decode() = 0;
	virtual void f_issue() = 0;
	virtual void f_execute() = 0;
	virtual void f_retire() = 0;

	virtual c_Transaction* f_retireTransaction(t_PortNo p_portNo,  t_ResponseType p_respType) = 0;
	virtual void f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction) = 0;
	virtual bool f_getModuleDone() = 0;
	virtual bool f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType) = 0;
	virtual void f_watchDogTimer() = 0;
};
#endif