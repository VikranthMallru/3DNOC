#include "AddressLogger.hpp"
#include "Controller.hpp"
#include "LogConfig.hpp"
#include <fstream>
#include <assert.h>

ofstream *c_AddressLogger::m_logFile = NULL;
c_LogConfig *c_AddressLogger::m_logConfig = NULL;

c_AddressLogger::c_AddressLogger()  {}

void c_AddressLogger::initLogger(std::string p_logConfigFile, c_LogConfig *p_logConfig) {
    if(m_logFile == NULL)   {
        m_logFile = new ofstream(p_logConfigFile);
        m_logConfig = p_logConfig;
    }
}

c_AddressLogger::~c_AddressLogger()  {
    if(m_logFile != NULL)   {
        delete m_logFile;
        m_logFile = NULL;
    }    
}

void c_AddressLogger::f_logDebug(c_Controller *p_ctrl, t_Address p_address,std::string p_message)    {
#ifdef VERBOSE_LOG
    if(m_logConfig->s_addressLog)   {
        if((m_logConfig->s_addressLogStartCycle == -1 || p_ctrl->f_currentCycle() >= m_logConfig->s_addressLogStartCycle) 
            && (p_ctrl->f_currentCycle() <= m_logConfig->s_addressLogEndCycle || m_logConfig->s_addressLogEndCycle == -1)) {
            if(m_logConfig->s_addressLogAddress == -1 || m_logConfig->s_addressLogAddress == p_address)    {
                std::string l_fullMessage = f_toString(p_ctrl,p_message);
                m_logFile->write(l_fullMessage.c_str(),l_fullMessage.length());
                m_logFile->flush();
            }
        }
    }
#endif
}

void c_AddressLogger::f_logRelease(c_Controller *p_ctrl, t_Address p_address,std::string p_message)    {
    std::string l_fullMessage = f_toString(p_ctrl,p_message);
    m_logFile->write(l_fullMessage.c_str(),l_fullMessage.length());
}

void c_AddressLogger::f_logError(c_Controller *p_ctrl, t_Address p_address,std::string p_message)    {
    std::string l_fullMessage = f_toString(p_ctrl,p_message);
    std::cout<<l_fullMessage<<std::endl;
    m_logFile->write(l_fullMessage.c_str(),l_fullMessage.length());
    m_logFile->flush();
    //fflush(stdout);
    assert(false);
}