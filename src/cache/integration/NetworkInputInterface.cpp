#include "NetworkInputInterface.hpp"
#include "RequestPool.hpp"
#include "NetworkPacket.hpp"
#include "MasterPort.hpp"
#include "SlavePort.hpp"
#include "TypeToString.hpp"
#include "Queue.hpp"
#include "TopologyConfig.hpp"
#include "SimConfig.hpp"
#include "Transaction.hpp"
#include "Topology.hpp"
#include "Switch.hpp"
#include "assert.h"

c_NetworkInputInterface::c_NetworkInputInterface(std::string p_name, c_TopologyConfig *p_topologyConfig, c_Topology *p_topology)  : c_Controller(p_name)  {
	try	{
        m_topologyConfig = p_topologyConfig;
        m_topology = p_topology;
        m_ctrl = this;
        m_numMemChannel = 1;
        m_clockCycle = 0;
        m_numCores = m_topology->f_getSimConfig()->s_numCores;
        std::string l_moduleName = p_name;
		m_switchInputQPE = new c_Queue**[p_topologyConfig->s_numChannels];
        m_switchInputQMM = new c_Queue**[p_topologyConfig->s_numChannels];
	    m_switchInputPortsPE = new c_MasterPort**[p_topologyConfig->s_numChannels];
        m_switchInputPortsMM = new c_MasterPort**[p_topologyConfig->s_numChannels];
        m_switchInputIssuedThisCycle = new bool*[p_topologyConfig->s_numChannels];
        for(t_NetworkChannel l_channel = 0; l_channel < p_topologyConfig->s_numChannels; l_channel++)   {
            m_switchInputQPE[l_channel] = new c_Queue*[p_topologyConfig->s_numNodes[l_channel]];
            m_switchInputQMM[l_channel] = new c_Queue*[p_topologyConfig->s_numNodes[l_channel]];
	        m_switchInputPortsPE[l_channel] = new c_MasterPort*[p_topologyConfig->s_numNodes[l_channel]];
            m_switchInputPortsMM[l_channel] = new c_MasterPort*[p_topologyConfig->s_numNodes[l_channel]];
            m_switchInputIssuedThisCycle[l_channel] = new bool[p_topologyConfig->s_numNodes[l_channel]];
            for(t_Size l_node = 0; l_node < p_topologyConfig->s_numNodes[l_channel]; l_node++) {
                std::string l_name = l_moduleName + "-nocInputInterfacePortPE-respType-" + c_TypeToString::f_responseTypeString((t_ResponseType)l_channel) + "-PortNo-" + std::to_string(l_node);
    			m_switchInputPortsPE[l_channel][l_node] = new c_MasterPort(m_ctrl, l_name,l_node, (t_ResponseType)l_channel);
                l_name = l_moduleName + "-nocInputInterfacePortCache-respType-" + c_TypeToString::f_responseTypeString((t_ResponseType)l_channel) + "-PortNo-" + std::to_string(p_topologyConfig->s_numNodes[l_channel] + l_node);
                l_name = l_moduleName + "-nocInputInterfacePortMM-respType-" + c_TypeToString::f_responseTypeString((t_ResponseType)l_channel) + "-PortNo-" + std::to_string(2* p_topologyConfig->s_numNodes[l_channel] + l_node);
                m_switchInputPortsMM[l_channel][l_node] = new c_MasterPort(m_ctrl, l_name,(2 * p_topologyConfig->s_numNodes[l_channel]) + l_node, (t_ResponseType)l_channel);
                m_switchInputQPE[l_channel][l_node] = new c_Queue(c_TypeToString::f_requestTypeString((t_RequestType)l_channel),(t_RequestType)l_channel, (t_ResponseType)l_channel, l_channel, p_topologyConfig->s_inputBufferSize[l_channel],t_QueueType::E_REQ_Q);
                m_switchInputQMM[l_channel][l_node] = new c_Queue(c_TypeToString::f_requestTypeString((t_RequestType)l_channel), (t_RequestType)l_channel, (t_ResponseType)l_channel, l_channel, p_topologyConfig->s_inputBufferSize[l_channel],t_QueueType::E_REQ_Q);
            }
        }
        
        for(t_Size l_reqType = E_CORE_READ; l_reqType < E_NUM_RECVQ_TYPE; l_reqType++) {
            m_coreOutputQ[l_reqType] = NULL;    
            m_coreOutputPorts[l_reqType] = NULL;
            m_memOutputQ[l_reqType] = NULL;
            m_memOutputPorts[l_reqType] = NULL;
        }
        m_coreOutputPorts[t_RequestType::E_CORE_READ] = new c_SlavePort*[p_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_CORE_READ)]];
        m_coreOutputQ[t_RequestType::E_CORE_READ] = new c_Queue*[p_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_CORE_READ)]];
        for(t_Size l_node = 0; l_node < p_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_CORE_READ)]; l_node++) {
            std::string l_name = l_moduleName + "-corePortNoC-reqType-" + c_TypeToString::f_requestTypeString(t_RequestType::E_CORE_READ) + "-PortNo-" + std::to_string(l_node);
			m_coreOutputQ[t_RequestType::E_CORE_READ][l_node] = new c_Queue(c_TypeToString::f_requestTypeString(t_RequestType::E_CORE_READ),t_RequestType::E_CORE_READ, t_ResponseType::E_NUM_SENDQ_TYPE, l_node, p_topologyConfig->s_inputBufferSize[f_mapReqTypeToChannel(t_RequestType::E_CORE_READ)],t_QueueType::E_REQ_Q);
	        m_coreOutputPorts[t_RequestType::E_CORE_READ][l_node] = new c_SlavePort(m_ctrl, l_name,l_node,t_RequestType::E_CORE_READ);
        }
        m_coreOutputPorts[t_RequestType::E_CORE_WRITE] = new c_SlavePort*[p_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_CORE_WRITE)]];
        m_coreOutputQ[t_RequestType::E_CORE_WRITE] = new c_Queue*[p_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_CORE_WRITE)]];
        for(t_Size l_node = 0; l_node < p_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_CORE_WRITE)]; l_node++) {
            std::string l_name = l_moduleName + "-corePortNoC-reqType-" + c_TypeToString::f_requestTypeString(t_RequestType::E_CORE_WRITE) + "-PortNo-" + std::to_string(l_node);
			m_coreOutputQ[t_RequestType::E_CORE_WRITE][l_node] = new c_Queue(c_TypeToString::f_requestTypeString(t_RequestType::E_CORE_WRITE),t_RequestType::E_CORE_WRITE, t_ResponseType::E_NUM_SENDQ_TYPE, l_node, p_topologyConfig->s_inputBufferSize[f_mapReqTypeToChannel(t_RequestType::E_CORE_WRITE)],t_QueueType::E_REQ_Q);
	        m_coreOutputPorts[t_RequestType::E_CORE_WRITE][l_node] = new c_SlavePort(m_ctrl, l_name,l_node,t_RequestType::E_CORE_WRITE);
        }
        m_coreOutputPorts[t_RequestType::E_PROBE_RESP] = new c_SlavePort*[p_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_PROBE_RESP)]];
        m_coreOutputQ[t_RequestType::E_PROBE_RESP] = new c_Queue*[p_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_PROBE_RESP)]];
        for(t_Size l_node = 0; l_node < p_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_PROBE_RESP)]; l_node++) {
            std::string l_name = l_moduleName + "-corePortNoC-reqType-" + c_TypeToString::f_requestTypeString(t_RequestType::E_PROBE_RESP) + "-PortNo-" + std::to_string(l_node);
			m_coreOutputQ[t_RequestType::E_PROBE_RESP][l_node] = new c_Queue(c_TypeToString::f_requestTypeString(t_RequestType::E_PROBE_RESP),t_RequestType::E_PROBE_RESP, t_ResponseType::E_NUM_SENDQ_TYPE, l_node, p_topologyConfig->s_inputBufferSize[f_mapReqTypeToChannel(t_RequestType::E_PROBE_RESP)],t_QueueType::E_REQ_Q);
	        m_coreOutputPorts[t_RequestType::E_PROBE_RESP][l_node] = new c_SlavePort(m_ctrl, l_name,l_node,t_RequestType::E_PROBE_RESP);
        }

        t_Size l_numNodes = p_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_PROBE_REQ)];
        m_memOutputQ[t_RequestType::E_PROBE_REQ] = new c_Queue*[l_numNodes];
        m_memOutputPorts[t_RequestType::E_PROBE_REQ] = new c_SlavePort*[l_numNodes];
        for(t_Size l_node = 0; l_node < l_numNodes; l_node++) {
            c_Switch *l_switch = m_topology->f_getSwitch(t_RequestType::E_PROBE_REQ,l_node);
                std::string l_name = l_moduleName + "-memPortNoC-reqType-" + c_TypeToString::f_requestTypeString(t_RequestType::E_PROBE_REQ) + "-PortNo-" + std::to_string(2 * l_numNodes + l_node);
                m_memOutputQ[t_RequestType::E_PROBE_REQ][l_node] = new c_Queue(c_TypeToString::f_requestTypeString(t_RequestType::E_PROBE_REQ),t_RequestType::E_PROBE_REQ, t_ResponseType::E_NUM_SENDQ_TYPE, 2 * l_numNodes + l_node, p_topologyConfig->s_inputBufferSize[f_mapReqTypeToChannel(E_PROBE_REQ)],t_QueueType::E_REQ_Q);
                m_memOutputPorts[t_RequestType::E_PROBE_REQ][l_node] = new c_SlavePort(m_ctrl, l_name,(2 * l_numNodes) + l_node,t_RequestType::E_PROBE_REQ);
            
        }
        l_numNodes = p_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_FILL)];
        m_memOutputQ[t_RequestType::E_FILL] = new c_Queue*[l_numNodes];
        m_memOutputPorts[t_RequestType::E_FILL] = new c_SlavePort*[l_numNodes];
        for(t_Size l_node = 0; l_node < l_numNodes; l_node++) {
            c_Switch *l_switch = m_topology->f_getSwitch(t_RequestType::E_FILL,l_node);
                std::string l_name = l_moduleName + "-memPortNoC-reqType-" + c_TypeToString::f_requestTypeString(t_RequestType::E_FILL) + "-PortNo-" + std::to_string(2 * l_numNodes + l_node);
                m_memOutputQ[t_RequestType::E_FILL][l_node] = new c_Queue(c_TypeToString::f_requestTypeString(t_RequestType::E_FILL),t_RequestType::E_FILL, t_ResponseType::E_NUM_SENDQ_TYPE, 2 * l_numNodes + l_node, p_topologyConfig->s_inputBufferSize[f_mapReqTypeToChannel(E_FILL)],t_QueueType::E_REQ_Q);
                m_memOutputPorts[t_RequestType::E_FILL][l_node] = new c_SlavePort(m_ctrl, l_name,(2 * l_numNodes) + l_node,t_RequestType::E_FILL);
            
        }
	}
    catch(const std::exception& e)    {
        std::cerr << e.what() << '\n';
        exit(-1);
    }
}

c_NetworkInputInterface::~c_NetworkInputInterface() {
	for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            delete m_switchInputPortsPE[l_channelCount][l_node];
            delete m_switchInputPortsMM[l_channelCount][l_node];
            delete m_switchInputQPE[l_channelCount][l_node];
            delete m_switchInputQMM[l_channelCount][l_node];
        }
        delete[] m_switchInputQPE[l_channelCount];
        delete[] m_switchInputPortsPE[l_channelCount];
        delete[] m_switchInputQMM[l_channelCount];
        delete[] m_switchInputPortsMM[l_channelCount];
        delete[] m_switchInputIssuedThisCycle[l_channelCount];
    }
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_CORE_READ)]; l_node++) {
        delete m_coreOutputPorts[t_RequestType::E_CORE_READ][l_node];
        delete m_coreOutputQ[t_RequestType::E_CORE_READ][l_node];
    }
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_CORE_WRITE)]; l_node++) {
        delete m_coreOutputPorts[t_RequestType::E_CORE_WRITE][l_node];
        delete m_coreOutputQ[t_RequestType::E_CORE_WRITE][l_node];
    }
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_PROBE_RESP)]; l_node++) {
        delete m_coreOutputPorts[t_RequestType::E_PROBE_RESP][l_node];
        delete m_coreOutputQ[t_RequestType::E_PROBE_RESP][l_node];
    }

    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_FILL)]; l_node++) {
        c_Switch *l_switch = m_topology->f_getSwitch(t_RequestType::E_FILL,l_node);
            delete m_memOutputPorts[t_RequestType::E_FILL][l_node];
            delete m_memOutputQ[t_RequestType::E_FILL][l_node];
    }
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_PROBE_REQ)]; l_node++) {
        c_Switch *l_switch = m_topology->f_getSwitch(t_RequestType::E_PROBE_REQ,l_node);
            delete m_memOutputPorts[t_RequestType::E_PROBE_REQ][l_node];
            delete m_memOutputQ[t_RequestType::E_PROBE_REQ][l_node];
    }
    
    delete[] m_coreOutputPorts[t_RequestType::E_CORE_READ];
    delete[] m_coreOutputPorts[t_RequestType::E_CORE_WRITE];
    delete[] m_coreOutputPorts[t_RequestType::E_PROBE_REQ];
    delete[] m_memOutputPorts[t_RequestType::E_PROBE_REQ];
    delete[] m_memOutputPorts[t_RequestType::E_FILL];
    delete[] m_coreOutputQ[t_RequestType::E_CORE_READ];
    delete[] m_coreOutputQ[t_RequestType::E_CORE_WRITE];
    delete[] m_coreOutputQ[t_RequestType::E_PROBE_REQ];
    delete[] m_memOutputQ[t_RequestType::E_FILL];
    delete[] m_memOutputQ[t_RequestType::E_PROBE_REQ];

    delete[] m_switchInputIssuedThisCycle;
    delete[] m_switchInputQPE;
    delete[] m_switchInputQMM;
    delete[] m_switchInputPortsPE;
    delete[] m_switchInputPortsMM;
}

c_MasterPort* c_NetworkInputInterface::f_getResponsePort(t_ResponseType p_responseType,t_PortNo p_portNo)   {
    t_NetworkChannel l_channel = f_mapRespTypeToChannel(p_responseType);
    t_NodeId l_node;
    c_MasterPort *l_port = NULL;
    if(p_portNo < m_topologyConfig->s_numNodes[l_channel])  {
        l_node = p_portNo;
        l_port = m_switchInputPortsPE[l_channel][l_node];
    }
    else {
        l_node = p_portNo - (2 * m_topologyConfig->s_numNodes[l_channel]);
        l_port = m_switchInputPortsMM[l_channel][l_node];
    }
    assert(l_port != NULL);
    return l_port;
}

c_SlavePort* c_NetworkInputInterface::f_getRequestPort(t_RequestType p_requestType,t_PortNo p_portNo)   {
    t_NodeId l_node;
    c_SlavePort *l_port = NULL;
    t_NetworkChannel l_channel = f_mapReqTypeToChannel(p_requestType);
    if(p_portNo < m_topologyConfig->s_numNodes[l_channel])  {
        l_node = p_portNo;
        l_port = m_coreOutputPorts[l_channel][l_node];
    }
    else {
        l_node = p_portNo - (2 * m_topologyConfig->s_numNodes[l_channel]);
        l_port = m_memOutputPorts[l_channel][l_node];
    }
    assert(l_port != NULL);
    return l_port;
}

t_Time c_NetworkInputInterface::f_currentCycle()    {
    return m_clockCycle;
}

void c_NetworkInputInterface::f_clockCycle()    {
    f_watchDogTimer();
    f_retire();
    f_execute();
    f_issue();
    f_decode();
    f_fetch();
    m_clockCycle++;
}

void c_NetworkInputInterface::f_resetClock()    {
    m_clockCycle = 0;
}

void c_NetworkInputInterface::f_fetch() {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            if(m_coreOutputPorts[l_channelCount] != NULL && m_coreOutputPorts[l_channelCount][l_node] != NULL)   {
                m_coreOutputPorts[l_channelCount][l_node]->f_clockCycle();
            }
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            if(m_memOutputPorts[l_channelCount] != NULL && m_memOutputPorts[l_channelCount][l_node] != NULL)    {
                m_memOutputPorts[l_channelCount][l_node]->f_clockCycle();
            }
        }
    }
}

void c_NetworkInputInterface::f_decode()    {}

void c_NetworkInputInterface::f_execute()   {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            if(m_coreOutputQ[l_channelCount] != NULL && m_coreOutputQ[l_channelCount][l_node] != NULL  && !m_coreOutputQ[l_channelCount][l_node]->f_empty())   {
                c_NetworkPacket *l_packet = (c_NetworkPacket *)m_coreOutputQ[l_channelCount][l_node]->f_get(0);
                c_Transaction *l_transaction = l_packet->f_getTransaction();
                l_transaction->f_convertProtocolRequestToResponse();    
            }
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            if(m_memOutputQ[l_channelCount] != NULL && m_memOutputQ[l_channelCount][l_node] != NULL && !m_memOutputQ[l_channelCount][l_node]->f_empty())    {
                c_NetworkPacket *l_packet = (c_NetworkPacket *)m_memOutputQ[l_channelCount][l_node]->f_get(0);
                c_Transaction *l_transaction = l_packet->f_getTransaction();
                l_transaction->f_convertProtocolRequestToResponse();
            }
        }
    }
}

void c_NetworkInputInterface::f_issue() {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            if(m_coreOutputPorts[l_channelCount] != NULL && m_coreOutputPorts[l_channelCount][l_node] != NULL)   {
                f_pushToNoC(m_switchInputQPE[l_channelCount][l_node], m_coreOutputQ[l_channelCount][l_node]);
            }
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            if(m_memOutputQ[l_channelCount] != NULL && m_memOutputQ[l_channelCount][l_node] != NULL)    {
                f_pushToNoC(m_switchInputQMM[l_channelCount][l_node], m_memOutputQ[l_channelCount][l_node]);
            }
        }
    }
}

void c_NetworkInputInterface::f_pushToNoC(c_Queue *p_nOCQueue, c_Queue *p_nIIQueue) {
    if(p_nOCQueue != NULL && p_nIIQueue != NULL && !p_nIIQueue->f_empty() && p_nOCQueue->f_hasCredit())    {
        c_NetworkPacket *l_packet = (c_NetworkPacket *)p_nIIQueue->f_get(0);
        c_Transaction *l_transaction = l_packet->f_getTransaction();
        p_nOCQueue->f_allocate(l_packet);
        p_nIIQueue->f_deallocate(l_packet);
        l_transaction->f_logDebug(this,"Pushed to switch Interface");
    }
}

t_NetworkChannel c_NetworkInputInterface::f_mapReqTypeToChannel(t_RequestType p_reqType)  {
    return (t_NetworkChannel) p_reqType;
}

t_NetworkChannel c_NetworkInputInterface::f_mapRespTypeToChannel(t_ResponseType p_respType)  {
    return (t_NetworkChannel) p_respType;
}

void c_NetworkInputInterface::f_retire()    {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            m_switchInputPortsPE[l_channelCount][l_node]->f_clockCycle();
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            if(m_switchInputPortsMM[l_channelCount] != NULL && m_switchInputPortsMM[l_channelCount][l_node] != NULL)    {
                m_switchInputPortsMM[l_channelCount][l_node]->f_clockCycle();
            }
        }
    }
}

void c_NetworkInputInterface::f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction)   {
    c_NetworkPacket *l_packet = c_RequestPool<c_NetworkPacket>::f_allocate(m_clockCycle);
    t_RequestType l_reqType = p_transaction->f_getRequestType();
    t_NetworkChannel l_channel = f_mapReqTypeToChannel(l_reqType);
    c_Queue *l_queue;
    t_NodeId l_node;
    std::string l_message;
    t_SwitchDirection l_sourceDirection;
    try {
        if(p_portNo < m_topologyConfig->s_numNodes[l_channel])  {
            l_node = p_portNo;
            l_queue = m_coreOutputQ[l_channel][l_node];
            l_sourceDirection = E_PE;
            l_message = "New request arrived from CORE to NII";
        }
        else {
            l_node = p_portNo - (2 * m_topologyConfig->s_numNodes[l_channel]);
            l_queue = m_memOutputQ[l_channel][l_node];
            l_sourceDirection = E_MM;
            l_message = "New request arrived from MEM to NII";
        }
                t_NodeId l_destinationNode;
                t_NodeId l_destination = m_topology->f_findDestination(p_transaction,l_sourceDirection,l_destinationNode);
                l_packet->f_initPacket(l_channel, l_node, l_destination, p_transaction, l_sourceDirection, m_clockCycle);
                l_queue->f_allocate(l_packet);
                l_packet->f_findDestinationDirectionForNoCache(p_transaction->f_getRequestType());
                p_transaction->f_setCurrentLevel(M_INSTALL_LEVEL_MAX);
                p_transaction->f_addNetworkPacket(l_packet);
                p_transaction->f_logDebug(this,l_message);
    }
	catch(const std::exception& e)    {
        p_transaction->f_logError(this,"Error fetching the transaction");
    }
}

c_Transaction* c_NetworkInputInterface::f_retireTransaction(t_PortNo p_portNo,t_ResponseType p_respType)    {
    c_Queue *l_queue = NULL;
    t_NetworkChannel l_channel = f_mapRespTypeToChannel(p_respType);
    t_PortNo l_node;
    if(p_portNo < m_topologyConfig->s_numNodes[l_channel])  {
        l_node = p_portNo;
        l_queue = m_switchInputQPE[l_channel][l_node];
    }
    else {
        l_node = p_portNo - 2 * m_topologyConfig->s_numNodes[l_channel];
        l_queue = m_switchInputQMM[l_channel][l_node];
    }
    
    c_Transaction *l_transaction = NULL;
    if(l_queue != NULL && !l_queue->f_empty()) {
        c_NetworkPacket *l_packet = (c_NetworkPacket *)l_queue->f_get(0);
        if(l_packet != NULL)    {
            l_transaction = l_packet->f_getTransaction();
            l_queue->f_deallocate(l_packet);
            l_transaction->f_logDebug(this,"Pushed to switch");
        }
    }
    return l_transaction;
}

std::string c_NetworkInputInterface::f_pipeString() {
    std::string l_pipeString = "NII: \n";
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        l_pipeString = l_pipeString + "Channel[" + std::to_string(l_channelCount) + "]:\n";
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            l_pipeString = l_pipeString + "Node[" + std::to_string(l_node) + "]: {PEOQ:[";
            for(t_Size l_index = 0; m_coreOutputQ[l_channelCount] != NULL && m_coreOutputQ[l_channelCount][l_node] != NULL && l_index < m_coreOutputQ[l_channelCount][l_node]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_coreOutputQ[l_channelCount][l_node]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
            l_pipeString = l_pipeString + "],MOQ:[";
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            for(t_Size l_index = 0; l_index < 1 && m_memOutputQ[l_channelCount] != NULL && m_memOutputQ[l_channelCount][l_node] != NULL && m_memOutputQ[l_channelCount][l_node]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_memOutputQ[l_channelCount][l_node]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
            l_pipeString = l_pipeString + "],SIQPE:[";
            for(t_Size l_index = 0; m_switchInputQPE[l_channelCount] != NULL && m_switchInputQPE[l_channelCount][l_node] != NULL && l_index < m_switchInputQPE[l_channelCount][l_node]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_switchInputQPE[l_channelCount][l_node]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
            l_pipeString = l_pipeString + "],SIQM:[";
            for(t_Size l_index = 0; m_switchInputQMM[l_channelCount] != NULL && m_switchInputQMM[l_channelCount][l_node] != NULL && l_index < m_switchInputQMM[l_channelCount][l_node]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_switchInputQMM[l_channelCount][l_node]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
            l_pipeString = l_pipeString + "]}\n";
        }
        l_pipeString = l_pipeString + "\n";
    }
    l_pipeString = l_pipeString + "\n";
    return l_pipeString;
}

bool c_NetworkInputInterface::f_getModuleDone() {
    bool l_done = true;
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            if(m_coreOutputQ[l_channelCount] != NULL && m_coreOutputQ[l_channelCount][l_node] != NULL)   {
                l_done = l_done & !m_coreOutputQ[l_channelCount][l_node]->f_empty();
            }
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            if(m_memOutputQ[l_channelCount] != NULL && m_memOutputQ[l_channelCount][l_node] != NULL)    {
                l_done = l_done & !m_memOutputQ[l_channelCount][l_node]->f_empty();
            }
            if(m_switchInputQPE[l_channelCount] != NULL && m_switchInputQPE[l_channelCount][l_node] != NULL)   {
                l_done = l_done & !m_switchInputQPE[l_channelCount][l_node]->f_empty();
            }
            if(m_switchInputQMM[l_channelCount] != NULL && m_switchInputQMM[l_channelCount][l_node] != NULL)    {
                l_done = l_done & !m_switchInputQMM[l_channelCount][l_node]->f_empty();
            }
        }
    }
    return l_done;
}

bool c_NetworkInputInterface::f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType)  {
    t_NodeId l_node;
    c_Queue *l_queue = NULL;
    if(p_portNo < m_topologyConfig->s_numNodes[p_nextReqType])  {
        l_node = p_portNo;
        l_queue = m_coreOutputQ[p_nextReqType][l_node];
    }
    else {
        l_node = p_portNo - (2 * m_topologyConfig->s_numNodes[p_nextReqType]);
        l_queue = m_memOutputQ[p_nextReqType][l_node];
    }
    assert(l_queue != NULL);
    return l_queue->f_hasCredit();
}

void c_NetworkInputInterface::f_watchDogTimer() {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            for(t_Size l_index = 0; m_coreOutputQ[l_channelCount] != NULL && m_coreOutputQ[l_channelCount][l_node] !=  NULL && l_index < m_coreOutputQ[l_channelCount][l_node]->f_size(); l_index++) {
                if(m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_coreOutputQ[l_channelCount][l_node]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_coreOutputQ[l_channelCount][l_node]->f_get(l_index))->f_logError(m_ctrl,"Recv Q Request Watchdog Timeout");
                }
            }
            for(t_Size l_index = 0; m_memOutputQ[l_channelCount] != NULL && m_memOutputQ[l_channelCount][l_node] != NULL && l_index < m_memOutputQ[l_channelCount][l_node]->f_size(); l_index++) {
                if(m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_memOutputQ[l_channelCount][l_node]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_memOutputQ[l_channelCount][l_node]->f_get(l_index))->f_logError(m_ctrl,"Recv Q Request Watchdog Timeout");
                }
            }
            for(t_Size l_index = 0; m_switchInputQPE[l_channelCount]  != NULL && m_switchInputQPE[l_channelCount][l_node] != NULL && l_index < m_switchInputQPE[l_channelCount][l_node]->f_size(); l_index++) {
                if(m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_switchInputQPE[l_channelCount][l_node]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_switchInputQPE[l_channelCount][l_node]->f_get(l_index))->f_logError(m_ctrl,"Send Q Response Watchdog Timeout");
                }
            }
            for(t_Size l_index = 0; m_switchInputQMM[l_channelCount] != NULL && m_switchInputQMM[l_channelCount][l_node] != NULL && l_index < m_switchInputQMM[l_channelCount][l_node]->f_size(); l_index++) {
                if(m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_switchInputQMM[l_channelCount][l_node]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_switchInputQMM[l_channelCount][l_node]->f_get(l_index))->f_logError(m_ctrl,"Send Q Response Watchdog Timeout");
                }
            }
        }
    }
}

void c_NetworkInputInterface::f_bindPorts(c_MasterPort *p_masterPort, c_SlavePort *p_slavePort)   {
    p_masterPort->f_bind(p_slavePort);
    p_slavePort->f_bind(p_masterPort);
    std::cout<<p_masterPort->f_toString()<<" <-> "<<p_slavePort->f_toString()<<std::endl;
    std::fflush(stdout);
}

void c_NetworkInputInterface::f_connect() {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            f_bindPorts(m_switchInputPortsPE[l_channelCount][l_node],l_switch->f_getSlavePort(t_SwitchDirection::E_PE));
            f_bindPorts(m_switchInputPortsMM[l_channelCount][l_node],l_switch->f_getSlavePort(t_SwitchDirection::E_MM));
        }
    }
}

void c_NetworkInputInterface::f_portCheck()  {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            m_switchInputPortsPE[l_channelCount][l_node]->f_portCheck();
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
                m_switchInputPortsMM[l_channelCount][l_node]->f_portCheck();
        }
    }
    m_memOutputPorts[E_FILL][0]->f_portCheck();
    m_memOutputPorts[E_PROBE_REQ][0]->f_portCheck();
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_CORE_READ)]; l_node++) {
        m_coreOutputPorts[E_CORE_READ][l_node]->f_portCheck();
    }
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_CORE_WRITE)]; l_node++) {
        m_coreOutputPorts[E_CORE_WRITE][l_node]->f_portCheck();
    }
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapReqTypeToChannel(t_RequestType::E_PROBE_RESP)]; l_node++) {
        m_coreOutputPorts[E_PROBE_RESP][l_node]->f_portCheck();
    }
}
