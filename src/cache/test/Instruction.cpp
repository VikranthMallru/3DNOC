#include "Instruction.hpp"
#include <assert.h>

c_Instruction::c_Instruction(c_Transaction *p_lsTransaction,t_Size p_noLSCount) {
    m_lsTransaction = p_lsTransaction;
    m_noLSCount = p_noLSCount;
}

t_Size c_Instruction::f_getNonLoadStore()   {
    return m_noLSCount;
}

c_Transaction* c_Instruction::f_getTransaction()   {
    return m_lsTransaction;
}

void c_Instruction::f_decNonLoadStore()    {
    assert(m_noLSCount != 0);
    m_noLSCount--;
}