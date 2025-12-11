#include "PipeView.hpp"
#include "LogConfig.hpp"
#include <iostream>
#include "Controller.hpp"
#include <fstream>
#include <assert.h>

c_PipeView::c_PipeView()  {
    m_pipeViewFile = NULL;
}

void c_PipeView::initPipeView(std::string p_pipeViewFile, c_LogConfig *p_logConfig) {
    m_pipeViewFile = new ofstream(p_pipeViewFile);
    m_logConfig = p_logConfig;
}

c_PipeView::~c_PipeView()  {
    if(m_pipeViewFile != NULL)  {
        m_pipeViewFile->close();
        delete m_pipeViewFile;
    }
    m_pipeViewFile = NULL;
}

void c_PipeView::f_logPipe(c_Controller *p_ctrl, std::string p_message)    {
#ifdef VERBOSE_LOG
    if(m_logConfig->s_pipeView)   {
        if((m_logConfig->s_pipeViewStartCycle == -1 || p_ctrl->f_currentCycle() >= m_logConfig->s_pipeViewStartCycle) 
            && (p_ctrl->f_currentCycle() <= m_logConfig->s_pipeViewEndCycle || m_logConfig->s_pipeViewEndCycle == -1)) {
                m_pipeViewFile->write(p_message.c_str(),p_message.length());
                m_pipeViewFile->flush();
            }
    }
#endif
}