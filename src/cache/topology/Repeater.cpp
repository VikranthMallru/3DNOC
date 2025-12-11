#include "SimConfig.hpp"
#include "Repeater.hpp"
#include "Coordinate.hpp"
#include "TopologyConfig.hpp"
#include "Topology.hpp"
#include "Routing.hpp"
#include "TypeToString.hpp"

c_Repeater::c_Repeater(c_Topology *p_topology, t_NodeId p_nodeId, c_Coordinate *p_coordinate, 
            t_NodeId *p_nodeIdNeighbours, t_Credit *p_bufferSize) : 
                c_Node(p_topology, p_nodeId, p_coordinate, p_nodeIdNeighbours, p_bufferSize, E_REPEATER) {
    m_ctrl = m_topology;
    m_masterPort = new c_MasterPort*[t_RepeaterDirection::E_MAX_DIR_REPEATER];
    m_slavePort = new c_SlavePort*[t_RepeaterDirection::E_MAX_DIR_REPEATER];
    m_inputBuffer = new c_Queue*[t_RepeaterDirection::E_MAX_DIR_REPEATER];
    m_outputBuffer = new c_Queue*[t_RepeaterDirection::E_MAX_DIR_REPEATER];
    
    for (unsigned l_direction = t_RepeaterDirection::E_PREV; l_direction < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_direction) {
        std::string l_nameMasterPort = "MasterPortSwitch-" + std::to_string(m_nodeId) + "-PortNo-" + std::to_string(l_direction);
        std::string l_nameSlavePort = "SlavePortSwitch-" + std::to_string(m_nodeId) + "-PortNo-" + std::to_string(l_direction);
		std::string l_nameInputQ = "InputQSwitch-" + std::to_string(m_nodeId) + "-PortNo-" + std::to_string(l_direction);
        std::string l_nameOutputQ = "OutputQSwitch-" + std::to_string(m_nodeId) + "-PortNo-" + std::to_string(l_direction);
		
        m_masterPort[l_direction] = new c_MasterPort(this,l_nameMasterPort, l_direction,t_ResponseType::E_NUM_SENDQ_TYPE);
        m_slavePort[l_direction] = new c_SlavePort(this,l_nameSlavePort, l_direction, t_RequestType::E_NUM_RECVQ_TYPE);
        m_inputBuffer[l_direction] = new c_Queue(l_nameInputQ, t_RequestType::E_NUM_RECVQ_TYPE, t_ResponseType::E_NUM_SENDQ_TYPE, l_direction, m_bufferSize[l_direction], t_QueueType::E_REQ_Q);
        m_outputBuffer[l_direction] = new c_Queue(l_nameOutputQ, t_RequestType::E_NUM_RECVQ_TYPE, t_ResponseType::E_NUM_SENDQ_TYPE, l_direction, m_bufferSize[l_direction], t_QueueType::E_RESP_Q);
    }
}

c_Repeater::~c_Repeater()    {
    for (unsigned l_direction = t_RepeaterDirection::E_PREV; l_direction < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_direction) {
        delete m_masterPort[l_direction];
        delete m_slavePort[l_direction];
        delete m_inputBuffer[l_direction];
        delete m_outputBuffer[l_direction];
    }
    delete[] m_masterPort;
    delete[] m_slavePort;
    delete[] m_inputBuffer;
    delete[] m_outputBuffer;
}

t_RepeaterDirection c_Repeater::f_oppositeDirection(t_RepeaterDirection p_direction) {
    switch(p_direction)    {
    case t_RepeaterDirection::E_NEXT:
        return t_RepeaterDirection::E_PREV;
    case t_RepeaterDirection::E_PREV:
        return t_RepeaterDirection::E_NEXT;
    }
}

void c_Repeater::f_fetch() {
    for (unsigned l_direction = t_RepeaterDirection::E_PREV; 
        l_direction < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_direction) {
        m_slavePort[l_direction]->f_clockCycle();
    }
}

void c_Repeater::f_decode() {}

void c_Repeater::f_issue() {
    for (unsigned l_dir = t_RepeaterDirection::E_PREV; l_dir < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_dir) {
        if (!m_inputBuffer[l_dir]->f_empty())   {
            c_NetworkPacket *l_packet = (c_NetworkPacket *) m_inputBuffer[l_dir]->f_get(0);
            t_RepeaterDirection l_destDirection = f_oppositeDirection((t_RepeaterDirection) l_dir);
            if(m_outputBuffer[l_destDirection] != NULL && m_outputBuffer[l_destDirection]->f_hasCredit()) {
                l_packet->f_setLastRepeaterDirection(l_destDirection);
                m_inputBuffer[l_dir]->f_deallocate(l_packet);
                m_outputBuffer[l_destDirection]->f_allocate(l_packet);
            }
        }
    }
}

void c_Repeater::f_execute() {
    for (unsigned l_dir = t_RepeaterDirection::E_PREV; 
            l_dir < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_dir) {
        if (m_outputBuffer[l_dir] != NULL && !m_outputBuffer[l_dir]->f_empty())   {
            c_NetworkPacket *l_packet = (c_NetworkPacket *) m_outputBuffer[l_dir]->f_get(0);
            c_Transaction *l_transaction = l_packet->f_getTransaction();
            l_transaction->f_convertProtocolRequestToResponse();
        }
    }
}

void c_Repeater::f_retire() {
    for (unsigned l_dir = t_RepeaterDirection::E_PREV; l_dir < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_dir) {
        m_masterPort[l_dir]->f_clockCycle();
    }
}

c_Transaction* c_Repeater::f_retireTransaction(t_PortNo p_portNo, t_ResponseType p_respType) {
    if (m_outputBuffer[p_portNo] != NULL && !m_outputBuffer[p_portNo]->f_empty()) {
        c_NetworkPacket *l_packet = (c_NetworkPacket *) m_outputBuffer[p_portNo]->f_get(0);
        m_outputBuffer[p_portNo]->f_deallocate(l_packet);
        return l_packet->f_getTransaction();
    }
    return NULL;
}

void c_Repeater::f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction) {
    c_NetworkPacket *l_packet = p_transaction->f_getNetworkPacket();
    l_packet->f_setCurrentNode(m_nodeId);
    m_inputBuffer[p_portNo]->f_allocate(l_packet);
}

void c_Repeater::f_clockCycle() {
    f_watchDogTimer();
    f_retire();
    f_execute();
    f_issue();
    f_decode();
    f_fetch();
    m_clockCycle++;
}

bool c_Repeater::f_getModuleDone() {
    bool l_isDone = true;
    for (unsigned l_direction = t_RepeaterDirection::E_PREV; l_direction < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_direction) {
        if (m_inputBuffer[l_direction] != NULL && !m_inputBuffer[l_direction]->f_empty()) {
            l_isDone = false;
            break;
        }
        if (m_outputBuffer[l_direction] != NULL && !m_outputBuffer[l_direction]->f_empty()) {
            l_isDone = false;
            break;
        }
    }
    return l_isDone;
}

std::string c_Repeater::f_pipeString()   {
    std::string l_pipeString = "Rep[" + std::to_string(m_nodeId) + "]:";
    for (unsigned l_direction = t_RepeaterDirection::E_PREV; l_direction < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_direction) {
        l_pipeString = l_pipeString + c_TypeToString::f_repeaterDirectionString((t_RepeaterDirection)l_direction) + "(IB: ";
        if (m_inputBuffer[l_direction] != NULL && !m_inputBuffer[l_direction]->f_empty()) {
            for(t_Size l_index = 0; l_index < m_inputBuffer[l_direction]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_inputBuffer[l_direction]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
        }
        l_pipeString = l_pipeString + ")(OB: ";
        if (m_outputBuffer[l_direction] != NULL && !m_outputBuffer[l_direction]->f_empty()) {
            for(t_Size l_index = 0; l_index < m_outputBuffer[l_direction]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_outputBuffer[l_direction]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
        }
        l_pipeString = l_pipeString + ")";
    }
    return l_pipeString;
}

void c_Repeater::f_watchDogTimer() {
    for (unsigned l_direction = t_RepeaterDirection::E_PREV; l_direction < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_direction) {
        if (m_inputBuffer[l_direction] != NULL && !m_inputBuffer[l_direction]->f_empty()) {
            for(t_Size l_index = 0; l_index < m_inputBuffer[l_direction]->f_size(); l_index++) {
                if(m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_inputBuffer[l_direction]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_inputBuffer[l_direction]->f_get(l_index))->f_logError(m_ctrl,"Recv Q Request Watchdog Timeout");
                }
            }
        }
        if (m_outputBuffer[l_direction] != NULL && !m_outputBuffer[l_direction]->f_empty()) {
            for(t_Size l_index = 0; l_index < m_outputBuffer[l_direction]->f_size(); l_index++) {
                if(m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_outputBuffer[l_direction]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_outputBuffer[l_direction]->f_get(l_index))->f_logError(m_ctrl,"Send Q Request Watchdog Timeout");
                }
            }
        }
    }
}

bool c_Repeater::f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType)   {
    return m_inputBuffer[p_portNo]->f_hasCredit();
}

void c_Repeater::f_portCheck()  {
    for(unsigned l_dir = E_PREV; l_dir < E_MAX_DIR_REPEATER; l_dir++)   {
        m_masterPort[l_dir]->f_portCheck();
        m_slavePort[l_dir]->f_portCheck();
    }
}

t_SwitchDirection c_Repeater::f_oppositeDirection(c_Node *p_Node, t_NodeId p_NodeId)    {
    for(unsigned l_dir = t_SwitchDirection::E_EAST; l_dir < t_SwitchDirection::E_MAX_DIR_SWITCH; l_dir++)  {
        if (p_Node->f_getNeighbour()[l_dir] == p_NodeId){
            return (t_SwitchDirection) l_dir;
        }
    }
    return t_SwitchDirection::E_MAX_DIR_SWITCH;
}
