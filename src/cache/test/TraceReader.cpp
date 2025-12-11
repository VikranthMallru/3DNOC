#include "TraceReader.hpp"
#include "CoreConfig.hpp"
#include "SimConfig.hpp"
#include "LoadStoreInterface.hpp"
#include "FetchInterface.hpp"
#include "MemoryHierarchy.hpp"
#include "Instruction.hpp"
#include "types.hpp"
#include "RequestPool.hpp"
#include "FixedLatencyStorage.hpp"
#include <assert.h>
#include <iostream>

c_TraceReader::c_TraceReader(c_SimConfig* p_simConfig, t_CoreID p_coreId, std::string p_name,std::string p_traceFile, 
    c_FetchInterface *p_fu,
    c_LoadStoreInterface *p_lsu, c_MemoryHierarchy *p_mh, c_FixedLatencyStorage *p_fls, bool p_warmpUp, t_CacheLevel p_warmupLevel) : 
    c_Controller(p_name) {
    m_coreId = p_coreId;
    m_traceFile = p_traceFile;
    m_traceInput.open(m_traceFile,std::fstream::in);
    m_warmup = p_warmpUp;
    m_warmupLevel = p_warmupLevel;
    m_traceDone = false;
    m_fu = p_fu;
    m_lsu = p_lsu;
    m_fls = p_fls;
    m_mh = p_mh;
    m_simConfig = p_simConfig;
    m_clockCycle = 0;
    m_instructionDecoded = 0;
    m_instructionIssued = 0;
    m_instructionRetired = 0;
    m_instructionBuffer = new std::queue<c_Instruction*>();
}

c_TraceReader::~c_TraceReader() {
    delete m_instructionBuffer;
}


t_Time c_TraceReader::f_currentCycle() {
    return m_clockCycle;
}

void c_TraceReader::f_resetClock()	{
	m_clockCycle = 0;
}

void c_TraceReader::f_clockCycle()  {
    if(m_mh->f_getSimConfig()->s_activeCores[m_coreId])    {
        f_execute();    
        f_issue();
        f_fetch();
    }
    m_clockCycle++;
}

void c_TraceReader::f_retire()  {}

void c_TraceReader::f_execute()  {
    m_lsu->f_clockCycle();
}

void c_TraceReader::f_issue()  {
    try {
        if(!m_instructionBuffer->empty())   {
            c_Instruction *l_instruction = m_instructionBuffer->front();
            if(l_instruction->f_getNonLoadStore() == 0)  {
                c_Transaction *l_transaction = l_instruction->f_getTransaction();
                bool l_pushed = false;
                l_transaction->f_logDebug(this,"Sending request to load store unit");
                if(l_transaction->f_getRequestType() == E_CORE_READ || l_transaction->f_getRequestType() == E_CORE_WRITE)   {
                    l_pushed = m_lsu->f_pushTransaction(l_transaction);
                }
                else if(l_transaction->f_getRequestType() == E_PROBE_REQ)   {
                    l_pushed = m_fls->f_pushTransaction(l_transaction);
                }
                else    {
                    l_transaction->f_logError(this,"Invalid transaction for trace player");
                }
                if(!l_pushed)   {
                    return;
                }
                m_instructionIssued++;
                m_instructionBuffer->pop();
                delete l_instruction;
            }
            else    {
                l_instruction->f_decNonLoadStore();
            }
        }
    }
	catch(const std::exception& e)    {
        f_logError(this,"Error while issuing in trace reader");
    }
}

bool c_TraceReader::f_canFetch()    {
    return m_instructionBuffer->size() < m_fu->f_getCoreConfig()->s_ibSize;
}

c_Transaction* c_TraceReader::f_readFile(t_Size &p_nonLsCount)  {
    std::string l_transactionStr;
    std::getline(m_traceInput, l_transactionStr);
    if(l_transactionStr == "")  {
        return NULL;
    }
    std::string l_delimiter = " ";
    size_t l_pos = 0;
    std::string l_token;
    unsigned int l_count = 0;
    t_VirtualAddress l_address;
    t_RequestType l_reqType;
    t_ResponseType l_respType;
    t_Time l_traceTime;
    t_Data l_data;
    t_CacheState l_state;

    while ((l_pos = l_transactionStr.find(l_delimiter)) != std::string::npos) {
        l_token = l_transactionStr.substr(0, l_pos);
        const char* l_value = l_token.c_str();
        char* l_end;
        if(l_count == 0)    {
            p_nonLsCount = strtol(l_value, &l_end, 0);
        }
        else if(l_count == 1)    {
            if(l_value[0] == 'R' || m_warmup)   {
                l_reqType = t_RequestType::E_CORE_READ;
                l_respType = t_ResponseType::E_NUM_SENDQ_TYPE;
                //l_state = t_CacheState::E_SHARED; //Enable for C2X tests
                l_state = t_CacheState::E_EXCLUSIVE;
            }
            else if(l_value[0] == 'W') {
                l_reqType = t_RequestType::E_CORE_WRITE;
                l_respType = t_ResponseType::E_NUM_SENDQ_TYPE;
                l_state = t_CacheState::E_MODIFIED;
            }
            else if(l_value[0] == 'P') {
                l_reqType = t_RequestType::E_PROBE_REQ;
                l_respType = t_ResponseType::E_PROBE_RESP_SEND;
                l_state = t_CacheState::E_INVALID;
            }
            else        {
                assert(false);
            }
        }
        else if(l_count == 2)    {
            l_address = strtol(l_value, &l_end, 0);
        }
        else if(l_count == 3)    {
            l_data = new unsigned char[64];
            std::memcpy(l_data, l_value, 64);
        }
        l_transactionStr.erase(0, l_pos + l_delimiter.length());
        l_count++;
    }
    c_Transaction *l_transaction = c_RequestPool<c_Transaction>::f_allocate(m_clockCycle);
    try {
        l_address = l_address | (m_coreId << 56); // Address Munging
        if(m_warmup)    {
            l_transaction->f_initTransaction(m_simConfig->s_numCores,l_address,l_reqType,l_data,l_respType,t_ProbeType::E_PRB_INVALIDATE,m_warmupLevel,0,l_state, m_clockCycle,m_coreId);
        }
        else {
            l_transaction->f_initTransaction(m_simConfig->s_numCores,l_address,l_reqType,l_data,l_respType,t_ProbeType::E_PRB_INVALIDATE,0,0,l_state, m_clockCycle,m_coreId);
        }
        l_transaction->f_setLineType(E_DATA);
    }
    catch(const std::exception& e)    {
        f_logError(this,"Error while decoding in trace reader");
    }    
    /*if(l_reqType == t_RequestType::E_CORE_READ && !m_warmup)  {
        l_transaction->f_setChangeToXType(t_ChangeToXType::E_C2X_REQ);
        l_transaction->f_setRequestCacheState(t_CacheState::E_EXCLUSIVE);
    }*/ // Enable only for C2X test
    return l_transaction;
}

void c_TraceReader::f_decode()  {
    t_Size l_nonLsCount;
    c_Transaction *l_transaction = f_readFile(l_nonLsCount);
    l_nonLsCount = 0; // made intentionally zero for easy simulation
    if(l_transaction != NULL)   {
        c_Instruction *l_instruction = new c_Instruction(l_transaction,l_nonLsCount);
        l_transaction->f_logDebug(this,"Creating a transaction");
        m_instructionBuffer->push(l_instruction);
        m_instructionDecoded++;
    }
}

void c_TraceReader::f_fetch()  {
    m_fu->f_clockCycle();
}

void c_TraceReader::f_warmUp()  {
    t_Size l_nonLsCount;
    c_Transaction *l_transaction = NULL;
    if(m_warmup)    {
        printf("Start Warming Up Cache\n");
        do
        {
            l_transaction = f_readFile(l_nonLsCount);
            if(l_transaction != NULL)   {
                m_mh->f_warmUp(l_transaction);
            }
        } while (l_transaction != NULL);
        printf("End Warming Up Cache\n");
        m_traceInput.close();
        m_traceInput.open(m_traceFile,std::fstream::in);
    }  
}

bool c_TraceReader::f_getModuleDone()   {
    return m_traceDone && m_lsu->f_getModuleDone();
}

void c_TraceReader::f_displayStats()    {
    std::cout<<"Decoded:"<<m_instructionDecoded<<"\tIssued:"<<m_instructionIssued<<"\tRetired"<<m_instructionRetired<<std::endl;
}
