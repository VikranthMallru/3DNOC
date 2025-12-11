#include <fstream>
#include <list>

#ifndef PIPEVIEW_H
#define PIPEVIEW_H

using namespace std;

class c_Controller;
class c_LogConfig;

class c_PipeView   {
protected:
    ofstream *m_pipeViewFile;
    c_LogConfig *m_logConfig;
public:
    c_PipeView();
    ~c_PipeView();
    void f_logPipe(c_Controller *p_ctrl, std::string p_message);
    virtual std::string f_pipeString() = 0;
    void initPipeView(std::string p_pipeViewFile, c_LogConfig *p_logConfig);
};

#endif