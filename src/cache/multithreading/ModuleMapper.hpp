#include "MemoryHierarchy.hpp"
#include "Controller.hpp"
#include <vector>

class c_ModuleMapper {
    private:
        std::vector<c_Controller*> m_modules;
        t_Size m_numSimThreads;
        t_Counter m_simThreadId;
    public:
        c_ModuleMapper(std::string p_name, t_Size p_numSimThreads, t_Counter p_simThreadId);
        void f_clockCycle();
        void f_addToThreads(c_Controller *p_ctrl);
        t_Counter f_getLoad();
        bool f_getModuleDone();
};