#include "NetworkOutputInterface.hpp"
#include "RequestPool.hpp"
#include "NetworkPacket.hpp"
#include "MasterPort.hpp"
#include "SlavePort.hpp"
#include "TypeToString.hpp"
#include "Queue.hpp"
#include "TopologyConfig.hpp"
#include "SimConfig.hpp"
#include "Topology.hpp"
#include "Switch.hpp"
#include "Transaction.hpp"
#include <assert.h>

c_NetworkOutputInterface::c_NetworkOutputInterface(std::string p_name, c_TopologyConfig *p_topologyConfig, c_Topology *p_topology)  : c_Controller(p_name)  {
	try	{
        m_clockCycle = 0;
        m_topologyConfig = p_topologyConfig;
        m_numMemChannel = 1;
        m_topology = p_topology;
        m_ctrl = this;
        std::string l_moduleName = p_name;
	    m_switchOutputQPE = new c_Queue**[p_topologyConfig->s_numChannels];
        m_switchOutputQMM = new c_Queue**[p_topologyConfig->s_numChannels];
	    m_switchOutputPortsPE = new c_SlavePort**[p_topologyConfig->s_numChannels];
        m_switchOutputPortsMM = new c_SlavePort**[p_topologyConfig->s_numChannels];
        m_switchOutputIssuedThisCycle = new bool*[p_topologyConfig->s_numChannels];
        for(t_Size l_channel = 0; l_channel < m_topologyConfig->s_numChannels; l_channel++) {
            m_switchOutputQPE[l_channel] = new c_Queue*[p_topologyConfig->s_numNodes[l_channel]];
            m_switchOutputQMM[l_channel] = new c_Queue*[p_topologyConfig->s_numNodes[l_channel]];
	        m_switchOutputPortsPE[l_channel] = new c_SlavePort*[p_topologyConfig->s_numNodes[l_channel]];
            m_switchOutputPortsMM[l_channel] = new c_SlavePort*[p_topologyConfig->s_numNodes[l_channel]];
            m_switchOutputIssuedThisCycle[l_channel] = new bool[p_topologyConfig->s_numNodes[l_channel]];
            for(t_Size l_node = 0; l_node < p_topologyConfig->s_numNodes[l_channel]; l_node++) {
                std::string l_name = l_moduleName + "-nocInterfacePEPort-respType-" + c_TypeToString::f_responseTypeString((t_ResponseType)l_channel) + "-PortNo-" + std::to_string(l_node);
			    m_switchOutputPortsPE[l_channel][l_node] = new c_SlavePort(m_ctrl, l_name,l_node, (t_RequestType)l_channel);
                l_name = l_moduleName + "-nocInterfaceCachePort-respType-" + c_TypeToString::f_responseTypeString((t_ResponseType)l_channel) + "-PortNo-" + std::to_string(p_topologyConfig->s_numNodes[l_channel] + l_node);
                c_Switch *l_switch = m_topology->f_getSwitch(l_channel,l_node);
                    l_name = l_moduleName + "-nocInterfaceMMPort-respType-" + c_TypeToString::f_responseTypeString((t_ResponseType)l_channel) + "-PortNo-" + std::to_string(2 * p_topologyConfig->s_numNodes[l_channel] + l_node);
                m_switchOutputPortsMM[l_channel][l_node] = new c_SlavePort(m_ctrl, l_name,2 * p_topologyConfig->s_numNodes[l_channel] + l_node, (t_RequestType)l_channel);
                m_switchOutputQMM[l_channel][l_node] = new c_Queue(c_TypeToString::f_responseTypeString((t_ResponseType)l_channel),(t_RequestType) l_channel, (t_ResponseType)l_channel, l_channel, p_topologyConfig->s_outputBufferSize[l_channel],t_QueueType::E_REQ_Q);
                m_switchOutputQPE[l_channel][l_node] = new c_Queue(c_TypeToString::f_responseTypeString((t_ResponseType)l_channel),(t_RequestType) l_channel, (t_ResponseType)l_channel, l_channel, p_topologyConfig->s_outputBufferSize[l_channel],t_QueueType::E_REQ_Q);
            }
        }

        for(t_Size l_respType = E_MISS; l_respType < E_NUM_SENDQ_TYPE; l_respType++) {
            m_coreInputQ[l_respType] = NULL;    
            m_coreInputPorts[l_respType] = NULL;
            m_memInputQ[l_respType] = NULL;
            m_memInputPorts[l_respType] = NULL;
        }
        t_NodeId l_numReadRespNodes = p_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_READ_RESP)];
        m_coreInputPorts[t_ResponseType::E_READ_RESP] = new c_MasterPort*[l_numReadRespNodes];
        m_coreInputQ[t_ResponseType::E_READ_RESP] = new c_Queue*[l_numReadRespNodes];
        for(t_Size l_node = 0; l_node < l_numReadRespNodes; l_node++) {
            std::string l_name = l_moduleName + "-corePortNoC-respType-" + c_TypeToString::f_responseTypeString(t_ResponseType::E_READ_RESP) + "-PortNo-" + std::to_string(l_node);
			m_coreInputQ[t_ResponseType::E_READ_RESP][l_node] = new c_Queue(c_TypeToString::f_responseTypeString(t_ResponseType::E_READ_RESP),t_RequestType::E_NUM_RECVQ_TYPE, t_ResponseType::E_READ_RESP, l_node, p_topologyConfig->s_outputBufferSize[f_mapRespTypeToChannel(t_ResponseType::E_READ_RESP)],t_QueueType::E_REQ_Q);
	        m_coreInputPorts[t_ResponseType::E_READ_RESP][l_node] = new c_MasterPort(m_ctrl, l_name,l_node,t_ResponseType::E_READ_RESP);
        }
        t_NodeId l_numProbeReqNodes = p_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_PROBE_REQ_SEND)];
        m_coreInputPorts[t_ResponseType::E_PROBE_REQ_SEND] = new c_MasterPort*[l_numProbeReqNodes];
        m_coreInputQ[t_ResponseType::E_PROBE_REQ_SEND] = new c_Queue*[l_numProbeReqNodes];
        for(t_Size l_node = 0; l_node < l_numProbeReqNodes; l_node++) {
            std::string l_name = l_moduleName + "-corePortNoC-respType-" + c_TypeToString::f_responseTypeString(t_ResponseType::E_PROBE_REQ_SEND) + "-PortNo-" + std::to_string(l_node);
			m_coreInputQ[t_ResponseType::E_PROBE_REQ_SEND][l_node] = new c_Queue(c_TypeToString::f_responseTypeString(t_ResponseType::E_PROBE_REQ_SEND),t_RequestType::E_NUM_RECVQ_TYPE, t_ResponseType::E_PROBE_REQ_SEND, l_node, p_topologyConfig->s_outputBufferSize[f_mapRespTypeToChannel(t_ResponseType::E_PROBE_REQ_SEND)],t_QueueType::E_REQ_Q);
	        m_coreInputPorts[t_ResponseType::E_PROBE_REQ_SEND][l_node] = new c_MasterPort(m_ctrl, l_name,l_node,t_ResponseType::E_PROBE_REQ_SEND);
        }
        
        t_Size l_numNodes = p_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_PROBE_RESP_SEND)];
        m_memInputQ[t_ResponseType::E_PROBE_RESP_SEND] = new c_Queue*[l_numNodes];
        m_memInputPorts[t_ResponseType::E_PROBE_RESP_SEND] = new c_MasterPort*[l_numNodes];
        for(t_Size l_node = 0; l_node < l_numNodes; l_node++) {
            c_Switch *l_switch = m_topology->f_getSwitch(t_ResponseType::E_PROBE_RESP_SEND,l_node);
            std::string l_name = l_moduleName + "-memPortNoC-respType-" + c_TypeToString::f_responseTypeString(t_ResponseType::E_PROBE_RESP_SEND) + "-PortNo-" + std::to_string(2 * l_numNodes + l_node);
                m_memInputQ[t_ResponseType::E_PROBE_RESP_SEND][l_node] = new c_Queue(c_TypeToString::f_responseTypeString(t_ResponseType::E_PROBE_RESP_SEND),t_RequestType::E_NUM_RECVQ_TYPE, t_ResponseType::E_NUM_SENDQ_TYPE, 2 * l_numNodes + l_node, p_topologyConfig->s_outputBufferSize[f_mapRespTypeToChannel(t_ResponseType::E_PROBE_RESP_SEND)],t_QueueType::E_REQ_Q);
                m_memInputPorts[t_ResponseType::E_PROBE_RESP_SEND][l_node] = new c_MasterPort(m_ctrl, l_name,(2 * l_numNodes) + l_node,t_ResponseType::E_PROBE_RESP_SEND);
            
        }
        l_numNodes = p_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_MISS)];
        m_memInputQ[t_ResponseType::E_MISS] = new c_Queue*[l_numNodes];
        m_memInputPorts[t_ResponseType::E_MISS] = new c_MasterPort*[l_numNodes];
        for(t_Size l_node = 0; l_node < l_numNodes; l_node++) {
            c_Switch *l_switch = m_topology->f_getSwitch(t_ResponseType::E_MISS,l_node);
                std::string l_name = l_moduleName + "-memPortNoC-respType-" + c_TypeToString::f_responseTypeString(t_ResponseType::E_MISS) + "-PortNo-" + std::to_string(2 * l_numNodes + l_node);
                m_memInputQ[t_ResponseType::E_MISS][l_node] = new c_Queue(c_TypeToString::f_responseTypeString(t_ResponseType::E_MISS),t_RequestType::E_NUM_RECVQ_TYPE, t_ResponseType::E_MISS, 2 * l_numNodes + l_node, p_topologyConfig->s_outputBufferSize[f_mapRespTypeToChannel(t_ResponseType::E_MISS)],t_QueueType::E_REQ_Q);
                m_memInputPorts[t_ResponseType::E_MISS][l_node] = new c_MasterPort(m_ctrl, l_name,(2 * l_numNodes) + l_node,t_ResponseType::E_MISS);
            
        }
        l_numNodes = p_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_VICTIM)];
        m_memInputQ[t_ResponseType::E_VICTIM] = new c_Queue*[l_numNodes];
        m_memInputPorts[t_ResponseType::E_VICTIM] = new c_MasterPort*[l_numNodes];
        for(t_Size l_node = 0; l_node < l_numNodes; l_node++) {
            c_Switch *l_switch = m_topology->f_getSwitch(t_ResponseType::E_VICTIM,l_node);
                std::string l_name = l_moduleName + "-memPortNoC-respType-" + c_TypeToString::f_responseTypeString(t_ResponseType::E_VICTIM) + "-PortNo-" + std::to_string(2 * l_numNodes + l_node);
                m_memInputQ[t_ResponseType::E_VICTIM][l_node] = new c_Queue(c_TypeToString::f_responseTypeString(t_ResponseType::E_VICTIM),t_RequestType::E_NUM_RECVQ_TYPE, t_ResponseType::E_VICTIM, 2 * l_numNodes + l_node, p_topologyConfig->s_outputBufferSize[f_mapRespTypeToChannel(t_ResponseType::E_VICTIM)],t_QueueType::E_REQ_Q);
                m_memInputPorts[t_ResponseType::E_VICTIM][l_node] = new c_MasterPort(m_ctrl, l_name,(2 * l_numNodes) + l_node,t_ResponseType::E_VICTIM);
            
        }
	}
    catch(const std::exception& e)    {
        std::cerr << e.what() << '\n';
        exit(-1);
    }
    assert(m_memInputPorts[t_ResponseType::E_MISS][0] != NULL);
}

c_NetworkOutputInterface::~c_NetworkOutputInterface() {
	for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            delete m_switchOutputPortsPE[l_channelCount][l_node];
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            delete m_switchOutputPortsMM[l_channelCount][l_node];
            delete m_switchOutputQPE[l_channelCount][l_node];
            delete m_switchOutputQMM[l_channelCount][l_node];
        }
        delete[] m_switchOutputQPE[l_channelCount];
        delete[] m_switchOutputQMM[l_channelCount];

        delete[] m_switchOutputPortsPE[l_channelCount];
        delete[] m_switchOutputPortsMM[l_channelCount];

        delete[] m_switchOutputIssuedThisCycle[l_channelCount];
    }

    t_NodeId l_numReadRespNodes = m_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_READ_RESP)];
    for(t_Size l_node = 0; l_node < l_numReadRespNodes; l_node++) {
        delete m_coreInputQ[t_ResponseType::E_READ_RESP][l_node];
	    delete m_coreInputPorts[t_ResponseType::E_READ_RESP][l_node];
    }
    
    t_NodeId l_numProbeReqNodes = m_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_PROBE_REQ_SEND)];
        for(t_Size l_node = 0; l_node < l_numProbeReqNodes; l_node++) {
    	delete m_coreInputQ[t_ResponseType::E_PROBE_REQ_SEND][l_node];
	    delete m_coreInputPorts[t_ResponseType::E_PROBE_REQ_SEND][l_node];
    }
        
    for(t_Size l_respType = E_MISS; l_respType < E_NUM_SENDQ_TYPE; l_respType++) {
        t_Size l_numNodes = m_topologyConfig->s_numNodes[f_mapRespTypeToChannel((t_ResponseType) l_respType)];
        for(t_Size l_node = 0; l_node < l_numNodes; l_node++) {
        }
    }

    t_Size l_numNodes = m_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_PROBE_RESP_SEND)];
    for(t_Size l_node = 0; l_node < l_numNodes; l_node++) {
        c_Switch *l_switch = m_topology->f_getSwitch(t_ResponseType::E_PROBE_RESP_SEND,l_node);
            delete m_memInputQ[t_ResponseType::E_PROBE_RESP_SEND][l_node];
            delete m_memInputPorts[t_ResponseType::E_PROBE_RESP_SEND][l_node];
    }
    
    l_numNodes = m_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_MISS)];
    for(t_Size l_node = 0; l_node < l_numNodes; l_node++) {
        c_Switch *l_switch = m_topology->f_getSwitch(t_ResponseType::E_MISS,l_node);
            delete m_memInputQ[t_ResponseType::E_MISS][l_node];
            delete m_memInputPorts[t_ResponseType::E_MISS][l_node];
    }
    l_numNodes = m_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_VICTIM)];
    for(t_Size l_node = 0; l_node < l_numNodes; l_node++) {
        c_Switch *l_switch = m_topology->f_getSwitch(t_ResponseType::E_VICTIM,l_node);
            delete m_memInputQ[t_ResponseType::E_VICTIM][l_node];
            delete m_memInputPorts[t_ResponseType::E_VICTIM][l_node];
    }

    delete[] m_coreInputPorts[t_ResponseType::E_READ_RESP];
    delete[] m_coreInputQ[t_ResponseType::E_READ_RESP];
    delete[] m_coreInputPorts[t_ResponseType::E_PROBE_REQ_SEND];
    delete[] m_coreInputQ[t_ResponseType::E_PROBE_REQ_SEND];
    delete[] m_memInputQ[t_ResponseType::E_PROBE_RESP_SEND];
    delete[] m_memInputPorts[t_ResponseType::E_PROBE_RESP_SEND];
    delete[] m_memInputQ[t_ResponseType::E_MISS];
    delete[] m_memInputPorts[t_ResponseType::E_MISS];
    delete[] m_memInputQ[t_ResponseType::E_VICTIM];
    delete[] m_memInputPorts[t_ResponseType::E_VICTIM];
    

    delete[] m_switchOutputIssuedThisCycle;
    delete[] m_switchOutputQPE;
    delete[] m_switchOutputQMM;

    delete[] m_switchOutputPortsPE;
    delete[] m_switchOutputPortsMM;
}

void c_NetworkOutputInterface::f_portCheck()  {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            m_switchOutputPortsPE[l_channelCount][l_node]->f_portCheck();
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            if(m_switchOutputPortsMM[l_channelCount][l_node] != NULL)    {
                m_switchOutputPortsMM[l_channelCount][l_node]->f_portCheck();
            }
        }
    }
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_PROBE_REQ_SEND)]; l_node++) {
        m_coreInputPorts[E_PROBE_REQ_SEND][l_node]->f_portCheck();
    }
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_READ_RESP)]; l_node++) {
        m_coreInputPorts[E_READ_RESP][l_node]->f_portCheck();
    }
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_PROBE_REQ_SEND)]; l_node++) {
        m_memInputPorts[E_MISS][l_node]->f_portCheck();
    }
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_PROBE_REQ_SEND)]; l_node++) {
        m_memInputPorts[E_VICTIM][l_node]->f_portCheck();
    }
    for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[f_mapRespTypeToChannel(t_ResponseType::E_PROBE_REQ_SEND)]; l_node++) {
        m_memInputPorts[E_PROBE_RESP_SEND][l_node]->f_portCheck();
    }

}

c_MasterPort* c_NetworkOutputInterface::f_getResponsePort(t_ResponseType p_responseType,t_PortNo p_portNo)   {
    t_NodeId l_node;
    if(p_portNo < m_topologyConfig->s_numNodes[p_responseType])  {
        l_node = p_portNo;
        return m_coreInputPorts[p_responseType][l_node];
    }
    else {
        l_node = p_portNo - 2 * m_topologyConfig->s_numNodes[p_responseType];
        return m_memInputPorts[p_responseType][l_node];
    }
}

c_SlavePort* c_NetworkOutputInterface::f_getRequestPort(t_RequestType p_requestType,t_PortNo p_portNo)   {
    t_NodeId l_node;
    c_SlavePort *l_port = NULL;
    if(p_portNo < m_topologyConfig->s_numNodes[p_requestType])  {
        l_node = p_portNo;
        l_port = m_switchOutputPortsPE[p_requestType][l_node];
    }
    else {
        l_node = p_portNo - (2 * m_topologyConfig->s_numNodes[p_requestType]);
        l_port = m_switchOutputPortsMM[p_requestType][l_node];
    }
    assert(l_port != NULL);
    return l_port;
}

t_Time c_NetworkOutputInterface::f_currentCycle()    {
    return m_clockCycle;
}

void c_NetworkOutputInterface::f_clockCycle()    {
    f_watchDogTimer();
    f_retire();
    f_execute();
    f_issue();
    f_decode();
    f_fetch();
    m_clockCycle++;
}

void c_NetworkOutputInterface::f_resetClock()    {
    m_clockCycle = 0;
}

void c_NetworkOutputInterface::f_fetch() {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            m_switchOutputPortsPE[l_channelCount][l_node]->f_clockCycle();
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            if(m_switchOutputPortsMM[l_channelCount] != NULL && m_switchOutputPortsMM[l_channelCount][l_node] != NULL)    {
                m_switchOutputPortsMM[l_channelCount][l_node]->f_clockCycle();
            }
        }
    }
}

void c_NetworkOutputInterface::f_decode()    {}

void c_NetworkOutputInterface::f_execute()   {}

void c_NetworkOutputInterface::f_issue() {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            if(m_coreInputQ[l_channelCount] != NULL && m_coreInputQ[l_channelCount][l_node] != NULL)   {
                f_pullFromNoC(m_switchOutputQPE[l_channelCount][l_node], m_coreInputQ[l_channelCount][l_node]);
            }
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            if(m_memInputQ[l_channelCount] != NULL && m_memInputQ[l_channelCount][l_node] != NULL)    {
                f_pullFromNoC(m_switchOutputQMM[l_channelCount][l_node], m_memInputQ[l_channelCount][l_node]);
            }
        }
    }
}

void c_NetworkOutputInterface::f_pullFromNoC(c_Queue *p_nOCQueue, c_Queue *p_nOIQueue) {
    if(p_nOCQueue != NULL && p_nOIQueue != NULL)    {
        if(!p_nOCQueue->f_empty())    {
            if(p_nOIQueue->f_hasCredit())   {
                c_NetworkPacket *l_packet = (c_NetworkPacket *)p_nOCQueue->f_get(0);
                c_Transaction *l_transaction = l_packet->f_getTransaction();
                l_transaction->f_convertProtocolRequestToResponse(); 
                p_nOIQueue->f_allocate(l_packet);
                p_nOCQueue->f_deallocate(l_packet);
                l_transaction->f_logDebug(this,"Issuing packet");
            }
        }
    }
    else
        assert(false);
}

t_NetworkChannel c_NetworkOutputInterface::f_mapRespTypeToChannel(t_ResponseType p_respType)  {
    return (t_NetworkChannel) p_respType;
}

t_NetworkChannel c_NetworkOutputInterface::f_mapReqTypeToChannel(t_RequestType p_requestType)  {
    return (t_NetworkChannel) p_requestType;
}

void c_NetworkOutputInterface::f_retire()    {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            if(m_coreInputPorts[l_channelCount] != NULL && m_coreInputPorts[l_channelCount][l_node] != NULL)    {
                m_coreInputPorts[l_channelCount][l_node]->f_clockCycle();
            }
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            if(m_memInputPorts[l_channelCount] != NULL && m_memInputPorts[l_channelCount][l_node] != NULL)    {
                m_memInputPorts[l_channelCount][l_node]->f_clockCycle();
            }
        }
    }
}

void c_NetworkOutputInterface::f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction)   {
    c_NetworkPacket *l_packet = p_transaction->f_getNetworkPacket();
    t_RequestType l_reqType = p_transaction->f_getRequestType();
    t_NetworkChannel l_channel = f_mapReqTypeToChannel(l_reqType);
    c_Queue *l_queue = NULL;
    t_NodeId l_node;
    std::string l_message;
    try {
		if(l_packet->f_getDestinationDirection() == t_SwitchDirection::E_PE)   {
            l_node = p_portNo;
            l_queue = m_switchOutputQPE[l_channel][l_node];
            l_message = "New request arrived from SWITCH to NOI directed to PE";
        }
        else if(l_packet->f_getDestinationDirection() == t_SwitchDirection::E_MM)   {
            l_node = p_portNo - 2 * m_topologyConfig->s_numNodes[l_channel];
            l_queue = m_switchOutputQMM[l_channel][l_node];
            l_message = "New request arrived from SWITCH to NOI directed to MM OUT";
        }
        else    {
            p_transaction->f_logError(this,"Unknown Direction");
        }
        l_queue->f_allocate(l_packet);
        l_packet->f_setOffRampTime(m_clockCycle);
        p_transaction->f_logDebug(this,l_message);
	}
	catch(const std::exception& e)    {
        p_transaction->f_logError(this,"Error fetching the transaction");
    }
}

c_Transaction* c_NetworkOutputInterface::f_retireTransaction(t_PortNo p_portNo,t_ResponseType p_respType)    {
    c_Queue *l_queue = NULL;
    t_NetworkChannel l_channel = f_mapRespTypeToChannel(p_respType);
    t_PortNo l_node;
    std::string l_message;
    if(p_portNo < m_topologyConfig->s_numNodes[l_channel])  {
        l_node = p_portNo;
        l_queue = m_coreInputQ[l_channel][l_node];
        l_message = "Pushed to Core";
    }
    else {
        l_node = p_portNo - 2 * m_topologyConfig->s_numNodes[l_channel];
        l_queue = m_memInputQ[l_channel][l_node];
        l_message = "Pushed to Memory";
    }
    
    c_Transaction *l_transaction = NULL;
    if(l_queue != NULL && !l_queue->f_empty()) {
        c_NetworkPacket *l_packet = (c_NetworkPacket *)l_queue->f_get(0);
        if(l_packet != NULL)    {
            l_transaction = l_packet->f_getTransaction();
            l_transaction->f_logDebug(this,l_message);
            l_transaction->f_addNetworkPacket(NULL);
            l_queue->f_deallocate(l_packet);
            c_RequestPool<c_NetworkPacket>::f_deAllocate(l_packet);
        }
    }
    return l_transaction;
}

std::string c_NetworkOutputInterface::f_pipeString() {
    std::string l_pipeString = "NOI: \n";
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        l_pipeString = l_pipeString + "Channel[" + std::to_string(l_channelCount) + "]:\n";
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            l_pipeString = l_pipeString + "Node[" + std::to_string(l_node) + "]: {PEIQ:[";
            for(t_Size l_index = 0; m_coreInputQ[l_channelCount] != NULL && m_coreInputQ[l_channelCount][l_node] != NULL && l_index < m_coreInputQ[l_channelCount][l_node]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_coreInputQ[l_channelCount][l_node]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
            l_pipeString = l_pipeString + "],MIQ:[";
            for(t_Size l_index = 0; m_memInputQ[l_channelCount] != NULL && m_memInputQ[l_channelCount][l_node] != NULL && l_index < m_memInputQ[l_channelCount][l_node]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_memInputQ[l_channelCount][l_node]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
            l_pipeString = l_pipeString + "],SOQPE:[";
            for(t_Size l_index = 0; m_switchOutputQPE[l_channelCount] != NULL && m_switchOutputQPE[l_channelCount][l_node] != NULL && l_index < m_switchOutputQPE[l_channelCount][l_node]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_switchOutputQPE[l_channelCount][l_node]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
            l_pipeString = l_pipeString + "],SOQM:[";
            for(t_Size l_index = 0; m_switchOutputQMM[l_channelCount] != NULL && m_switchOutputQMM[l_channelCount][l_node] != NULL && l_index < m_switchOutputQMM[l_channelCount][l_node]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_switchOutputQMM[l_channelCount][l_node]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
            l_pipeString = l_pipeString + "]}\n";
        }
        l_pipeString = l_pipeString + "\n";
    }
    l_pipeString = l_pipeString + "\n";
    return l_pipeString;
}

bool c_NetworkOutputInterface::f_getModuleDone() {
    bool l_done = true;
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            if(m_coreInputQ[l_channelCount] != NULL && m_coreInputQ[l_channelCount][l_node] != NULL)    {
                l_done = l_done & !m_coreInputQ[l_channelCount][l_node]->f_empty();
            }
            if(m_memInputQ[l_channelCount] != NULL && m_memInputQ[l_channelCount][l_node] != NULL)    {
                l_done = l_done & !m_memInputQ[l_channelCount][l_node]->f_empty();
            }
            if(m_switchOutputQPE[l_channelCount] != NULL && m_switchOutputQPE[l_channelCount][l_node] != NULL)    {
                l_done = l_done & !m_switchOutputQPE[l_channelCount][l_node]->f_empty();
            }
            if(m_switchOutputQMM[l_channelCount] != NULL && m_switchOutputQMM[l_channelCount][l_node] != NULL)    {
                l_done = l_done & !m_switchOutputQMM[l_channelCount][l_node]->f_empty();
            }
        }
    }
    return l_done;
}

bool c_NetworkOutputInterface::f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType)  {
    t_NetworkChannel l_channel = f_mapReqTypeToChannel(p_nextReqType);
    c_Queue *l_queue = NULL;
    t_NodeId l_node;
    if(p_portNo < m_topologyConfig->s_numNodes[l_channel])  {
        l_node = p_portNo;
        l_queue = m_switchOutputQPE[l_channel][l_node];
    }
    else {
        l_node = p_portNo - (2 * m_topologyConfig->s_numNodes[l_channel]);
        l_queue = m_switchOutputQMM[l_channel][l_node];
    }
    assert(l_queue != NULL);
    return l_queue->f_hasCredit(); 
}

void c_NetworkOutputInterface::f_watchDogTimer() {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            for(t_Size l_index = 0; m_coreInputQ[l_channelCount] != NULL && m_coreInputQ[l_channelCount][l_node] != NULL && l_index < m_coreInputQ[l_channelCount][l_node]->f_size(); l_index++) {
                if(m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_coreInputQ[l_channelCount][l_node]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_coreInputQ[l_channelCount][l_node]->f_get(l_index))->f_logError(m_ctrl,"Recv Q Request Watchdog Timeout");
                }
            }
            for(t_Size l_index = 0; m_memInputQ[l_channelCount] != NULL && m_memInputQ[l_channelCount][l_node] != NULL && l_index < m_memInputQ[l_channelCount][l_node]->f_size(); l_index++) {
                if(m_memInputQ[l_channelCount] != NULL && m_memInputQ[l_channelCount][l_node] != NULL && m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_memInputQ[l_channelCount][l_node]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_memInputQ[l_channelCount][l_node]->f_get(l_index))->f_logError(m_ctrl,"Recv Q Request Watchdog Timeout");
                }
            }
            for(t_Size l_index = 0; m_switchOutputQPE[l_channelCount] != NULL && m_switchOutputQPE[l_channelCount][l_node] != NULL && l_index < m_switchOutputQPE[l_channelCount][l_node]->f_size(); l_index++) {
                if(m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_switchOutputQPE[l_channelCount][l_node]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_switchOutputQPE[l_channelCount][l_node]->f_get(l_index))->f_logError(m_ctrl,"Send Q Request Watchdog Timeout");
                }
            }
            for(t_Size l_index = 0; m_switchOutputQMM[l_channelCount] != NULL && m_switchOutputQMM[l_channelCount][l_node] != NULL && l_index < m_switchOutputQMM[l_channelCount][l_node]->f_size(); l_index++) {
                if(m_switchOutputQMM[l_channelCount] != NULL && m_switchOutputQMM[l_channelCount][l_node] != NULL && m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_switchOutputQMM[l_channelCount][l_node]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_switchOutputQMM[l_channelCount][l_node]->f_get(l_index))->f_logError(m_ctrl,"Recv Q Request Watchdog Timeout");
                }
            }
        }
    }
}

void c_NetworkOutputInterface::f_bindPorts(c_MasterPort *p_masterPort, c_SlavePort *p_slavePort)   {
    p_masterPort->f_bind(p_slavePort);
    p_slavePort->f_bind(p_masterPort);
    std::cout<<p_masterPort->f_toString()<<" <-> "<<p_slavePort->f_toString()<<std::endl;
    std::fflush(stdout);
}

void c_NetworkOutputInterface::f_connect() {
    for(t_Size l_channelCount = 0; l_channelCount < m_topologyConfig->s_numChannels; l_channelCount++)	{
        for(t_Size l_node = 0; l_node < m_topologyConfig->s_numNodes[l_channelCount]; l_node++) {
            c_Switch *l_switch = m_topology->f_getSwitch(l_channelCount,l_node);
            f_bindPorts(l_switch->f_getMasterPort(t_SwitchDirection::E_PE), m_switchOutputPortsPE[l_channelCount][l_node]);
            if(m_switchOutputPortsMM[l_channelCount] != NULL && m_switchOutputPortsMM[l_channelCount][l_node] != NULL)    {
                f_bindPorts(l_switch->f_getMasterPort(t_SwitchDirection::E_MM), m_switchOutputPortsMM[l_channelCount][l_node]);
            }
        }
    }
}
