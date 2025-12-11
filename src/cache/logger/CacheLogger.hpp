#include <fstream>
#include <list>
#include "CacheLoggerType.hpp"
#include <iostream>

#ifndef CACHELOGGER_H
#define CACHELOGGER_H

using namespace std;

class c_Controller;
class c_LogConfig;

class c_CacheLogger   {
protected:
    static ofstream *m_logFile;
    t_LogType m_logType;
    t_PacketInfo m_packetInfo;
    static c_LogConfig *m_logConfig;
public:
    static bool s_logPort;
    static bool *m_logCacheController;
    static bool *m_logMetadataController;
    static bool *m_logMMController;
    static bool *m_logCoreController;
    static bool *m_networkController;

    c_CacheLogger();
    ~c_CacheLogger();
    void f_logDebug(c_Controller *p_ctrl,std::string p_message);
    void f_logError(c_Controller *p_ctrl,std::string p_message);
    void f_logRelease(c_Controller *p_ctrl,std::string p_message);
    virtual std::string f_toString(c_Controller *p_ctrl,std::string p_message) = 0;
    static void initLogger(std::string p_logConfigFile, c_LogConfig *p_logConfig);
};

#endif