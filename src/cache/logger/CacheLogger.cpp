#include "CacheLogger.hpp"
#include "Controller.hpp"
#include "LogConfig.hpp"
#include <fstream>
#include <assert.h>

ofstream *c_CacheLogger::m_logFile = NULL;
c_LogConfig *c_CacheLogger::m_logConfig = NULL;
bool c_CacheLogger::s_logPort = false;

c_CacheLogger::c_CacheLogger()  {}

void c_CacheLogger::initLogger(std::string p_logConfigFile, c_LogConfig *p_logConfig) {
    if(m_logFile == NULL)   {
        m_logFile = new ofstream(p_logConfigFile);
        m_logConfig = p_logConfig;
    }
}

c_CacheLogger::~c_CacheLogger()  {
    if(m_logFile != NULL)   {
        delete m_logFile;
        m_logFile = NULL;
    }    
}

void c_CacheLogger::f_logDebug(c_Controller *p_ctrl,std::string p_message)    {
#ifdef VERBOSE_LOG
    if(m_logConfig->s_transactionLog)   {
        if((m_logConfig->s_transactionLogStartCycle == -1 || p_ctrl->f_currentCycle() >= m_logConfig->s_transactionLogStartCycle) 
            && (p_ctrl->f_currentCycle() <= m_logConfig->s_transactionLogEndCycle || m_logConfig->s_transactionLogEndCycle == -1)) {
            std::string l_fullMessage = f_toString(p_ctrl,p_message);
            m_logFile->write(l_fullMessage.c_str(),l_fullMessage.length());
            m_logFile->flush();
        }
    }
#endif
}

void c_CacheLogger::f_logRelease(c_Controller *p_ctrl,std::string p_message)    {
    std::string l_fullMessage = f_toString(p_ctrl,p_message);
    m_logFile->write(l_fullMessage.c_str(),l_fullMessage.length());
}

void c_CacheLogger::f_logError(c_Controller *p_ctrl,std::string p_message)    {
    std::string l_fullMessage = f_toString(p_ctrl,p_message);
    std::cout<<l_fullMessage<<std::endl;
    m_logFile->write(l_fullMessage.c_str(),l_fullMessage.length());
    m_logFile->flush();
    //fflush(stdout);
    assert(false);
}