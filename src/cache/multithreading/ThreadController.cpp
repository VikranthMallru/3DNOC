#include "ThreadController.hpp"
#include "ModuleMapper.hpp"

t_Counter c_ThreadController::m_numSimThreads = 1;
c_ModuleMapper** c_ThreadController::m_mapper = NULL;

c_ThreadController::c_ThreadController(){
    //m_numSimThreads = std::thread::hardware_concurrency() - 1;
    m_mapper = new c_ModuleMapper*[m_numSimThreads]; 
    for(t_Counter l_simThread = 0; l_simThread < m_numSimThreads; l_simThread++){
        m_mapper[l_simThread] = new c_ModuleMapper("ThreadMapper_"+std::to_string(l_simThread), m_numSimThreads, l_simThread);
    }
}

c_ThreadController::~c_ThreadController()    {
    for(t_Counter l_simThread = 0; l_simThread < m_numSimThreads; l_simThread++){
        delete m_mapper[l_simThread];
    }
    delete[] m_mapper;
}

void c_ThreadController::f_addToThreads(c_Controller *p_ctrl)   {
    t_Counter l_minLoad = m_mapper[0]->f_getLoad();
    t_Counter l_minLoadThreadId = 0;
    for(t_Counter l_threadId = 1; l_threadId < m_numSimThreads; l_threadId++)   {
        if(m_mapper[l_threadId]->f_getLoad() < l_minLoad) {
            l_minLoad = m_mapper[l_threadId]->f_getLoad();
            l_minLoadThreadId = l_threadId;
        }
    }
    //printf("%s mapped to %d\n",p_ctrl->name(),l_minLoadThreadId);
    m_mapper[l_minLoadThreadId]->f_addToThreads(p_ctrl);
}

bool c_ThreadController::f_getModuleDone()  {
    bool l_moduleDone = true;
    for(t_Counter l_threadId = 1; l_threadId < m_numSimThreads; l_threadId++)   {
        l_moduleDone = l_moduleDone && m_mapper[l_threadId]->f_getModuleDone();
    }
    return l_moduleDone;
}

void c_ThreadController::f_start()  {
    for(t_Counter l_threadId = 0; l_threadId < m_numSimThreads; l_threadId++)   {
        m_mapper[l_threadId]->f_clockCycle();
    }
}