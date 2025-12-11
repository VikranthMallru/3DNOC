#include "types.hpp"

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

class c_Transaction;

class c_Instruction {
private:
    c_Transaction *m_lsTransaction;
    t_Size m_noLSCount;
public:
    c_Instruction(c_Transaction *p_lsTransaction,t_Size p_noLSCount);
    t_Size f_getNonLoadStore();
    void f_decNonLoadStore();
    c_Transaction* f_getTransaction();
};

#endif