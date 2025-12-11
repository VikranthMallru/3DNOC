#include <fstream>
#include <list>
#include "types.hpp"
#include "CacheLoggerType.hpp"
#include <iostream>

#ifndef ADDRESSLOGGER_H
#define ADDRESSLOGGER_H

using namespace std;

class c_Controller;
class c_LogConfig;

class c_AddressLogger   {
protected:
    static ofstream *m_logFile;
    static c_LogConfig *m_logConfig;
public:
    c_AddressLogger();
    ~c_AddressLogger();
    void f_logDebug(c_Controller *p_ctrl, t_Address p_address,std::string p_message);
    void f_logError(c_Controller *p_ctrl, t_Address p_address,std::string p_message);
    void f_logRelease(c_Controller *p_ctrl, t_Address p_address,std::string p_message);
    virtual std::string f_toString(c_Controller *p_ctrl,std::string p_message) = 0;
    static void initLogger(std::string p_logConfigFile, c_LogConfig *p_logConfig);
};

#endif