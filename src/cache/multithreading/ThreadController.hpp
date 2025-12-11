#include "MemoryHierarchy.hpp"
#include <thread>
#include "types.hpp"

class c_ModuleMapper;

class c_ThreadController{
private:
    static t_Counter m_numSimThreads;
    static c_ModuleMapper** m_mapper;
public:
    static t_Size m_numModules;
    
    c_ThreadController();
    ~c_ThreadController();
    void f_start();
    void f_addToThreads(c_Controller *p_ctrl);
    static bool f_getModuleDone();
};