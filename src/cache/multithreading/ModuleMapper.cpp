#include "ModuleMapper.hpp"
#include "ThreadController.hpp"
#include <vector>

c_ModuleMapper::c_ModuleMapper(std::string p_name, t_Size p_numSimThreads, t_Counter p_simThreadId) {
    m_numSimThreads = p_numSimThreads;
    m_simThreadId = p_simThreadId;
}

void c_ModuleMapper::f_clockCycle(){
    t_Counter l_countCycles = 0;
    while(l_countCycles != 100) {
        for (auto l_module = m_modules.begin(); l_module != m_modules.end(); ++l_module)    {
            (*l_module)->f_clockCycle();
        }
        if(f_getModuleDone())    {
            l_countCycles = l_countCycles + 1;
        }
        else    {
            l_countCycles = 0;
        }
    }
}

void c_ModuleMapper::f_addToThreads(c_Controller *p_ctrl)   {
    m_modules.push_back(p_ctrl);
}

t_Counter c_ModuleMapper::f_getLoad()   {
    t_Counter l_count = 0;
    for (auto l_module = m_modules.begin(); l_module != m_modules.end(); ++l_module)    {
        l_count++;
    }
    return l_count;
}

bool c_ModuleMapper::f_getModuleDone()  {
    bool l_moduleDone = true;
    for (auto l_module = m_modules.begin(); l_module != m_modules.end(); ++l_module)    {
        l_moduleDone = l_moduleDone && (*l_module)->f_getModuleDone();
    }
    return l_moduleDone;
}