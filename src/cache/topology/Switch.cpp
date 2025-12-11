#include "Switch.hpp"
#include "Coordinate.hpp"
#include "TopologyConfig.hpp"
#include "SimConfig.hpp"
#include "Topology.hpp"
#include "Routing.hpp"
#include "TypeToString.hpp"
#include "assert.h"

c_Switch::c_Switch(c_Topology *p_topology, t_NodeId p_nodeId, c_Coordinate *p_coordinate, 
            t_NodeId *p_nodeIdNeighbours, t_Credit *p_bufferSize) : 
                c_Node(p_topology, p_nodeId, p_coordinate, p_nodeIdNeighbours, p_bufferSize, E_SWITCH) {    
    m_ctrl = m_topology;
    m_lastDirection = t_SwitchDirection::E_PE;
    m_masterPort = new c_MasterPort*[t_SwitchDirection::E_MAX_DIR_SWITCH];
    m_slavePort = new c_SlavePort*[t_SwitchDirection::E_MAX_DIR_SWITCH];
    m_inputBuffer = new c_Queue*[t_SwitchDirection::E_MAX_DIR_SWITCH];
    m_outputBuffer = new c_Queue*[t_SwitchDirection::E_MAX_DIR_SWITCH];
    
    for (unsigned l_direction = t_SwitchDirection::E_PE; l_direction < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_direction) {
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

c_Switch::~c_Switch() {
    for (unsigned l_direction = t_SwitchDirection::E_PE; l_direction < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_direction) {
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

void c_Switch::f_fetch() {
    for (unsigned l_direction = t_SwitchDirection::E_PE; l_direction < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_direction) {
                m_slavePort[l_direction]->f_clockCycle();
    }
}

void c_Switch::f_decode() {}

t_SwitchDirection c_Switch::f_oppositeDirection(t_SwitchDirection p_direction) {
    switch(p_direction)    {
    case t_SwitchDirection::E_NORTH:
        return t_SwitchDirection::E_SOUTH;
    case t_SwitchDirection::E_SOUTH:
        return t_SwitchDirection::E_NORTH;
    case t_SwitchDirection::E_EAST:
        return t_SwitchDirection::E_WEST;
    case t_SwitchDirection::E_WEST:
        return t_SwitchDirection::E_EAST;
    case t_SwitchDirection::E_UP:
        return t_SwitchDirection::E_DOWN;
    case t_SwitchDirection::E_DOWN:
        return t_SwitchDirection::E_UP;
    }
}

void c_Switch::f_issue() {
    t_SwitchDirection l_nextDirectionRR = t_SwitchDirection::E_PE;
    bool l_outputSelected[t_SwitchDirection::E_MAX_DIR_SWITCH];
    bool l_outputSelectedMem = false;
    for (unsigned l_dir = t_SwitchDirection::E_PE; l_dir < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_dir) {
        l_outputSelected[l_dir] = false;
    }
    for (unsigned l_dir = t_SwitchDirection::E_PE;
            l_dir < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_dir) {
        l_nextDirectionRR = (t_SwitchDirection) ((m_lastDirection + l_dir + 1) % t_SwitchDirection::E_MAX_DIR_SWITCH);
        if (!m_inputBuffer[l_nextDirectionRR]->f_empty()) {
            c_NetworkPacket *l_packet = (c_NetworkPacket *) m_inputBuffer[l_nextDirectionRR]->f_get(0);
            if(l_packet != NULL)    { 
                c_Transaction *l_transaction = l_packet->f_getTransaction();
                if(!isCurrentNodeDestination(l_packet))    {
                    t_SwitchDirection l_destDirection = (t_SwitchDirection) m_topology->f_getRoutingTable()->f_getDirection(l_packet);
                    if(!l_outputSelected[l_destDirection] && m_outputBuffer[l_destDirection]->f_hasCredit())  {
                        m_lastDirection = l_nextDirectionRR;
                        m_inputBuffer[l_nextDirectionRR]->f_deallocate(l_packet);
                        m_outputBuffer[l_destDirection]->f_allocate(l_packet);
                        l_packet->f_setLastSwitchDirection(m_lastDirection);
                        l_transaction->f_logDebug(m_topology,"Switch Output Buffer directing to next hop");
                        l_outputSelected[l_destDirection] = true;
                    }
                }
                else    {
                    t_SwitchDirection l_destDirection = l_packet->f_getDestinationDirection();
                    if(!l_outputSelected[l_destDirection] && m_outputBuffer[l_destDirection]->f_hasCredit())  {
                        m_lastDirection = l_nextDirectionRR;
                        m_outputBuffer[l_destDirection]->f_allocate(l_packet);
                        m_inputBuffer[l_nextDirectionRR]->f_deallocate(l_packet);
                        l_packet->f_setLastSwitchDirection(m_lastDirection);
                        l_transaction->f_logDebug(m_topology,"Switch Output Buffer directing to " + c_TypeToString::f_switchDirectionString(l_destDirection));
                        l_outputSelected[l_destDirection] = true;
                    }
                }
            }
        }
    }
    // if(m_isMemConnected && !m_memInputBuffer->f_empty())    {
    //     c_NetworkPacket *l_packet = (c_NetworkPacket *) m_memInputBuffer->f_get(0);
    //     if(l_packet != NULL)    { 
    //         c_Transaction *l_transaction = l_packet->f_getTransaction();
    //         if(!isCurrentNodeDestination(l_packet))    {
    //             t_SwitchDirection l_destDirection = (t_SwitchDirection) m_topology->f_getRoutingTable()->f_getDirection(l_packet);
    //             if(!l_outputSelected[l_destDirection] && m_outputBuffer[l_destDirection]->f_hasCredit())  {
    //                 m_memInputBuffer->f_deallocate(l_packet);
    //                 m_outputBuffer[l_destDirection]->f_allocate(l_packet);
    //                 l_packet->f_setLastSwitchDirection(E_MM_IN);
    //                 l_transaction->f_logDebug(m_topology,"Switch Output Buffer directing to next hop");
    //                 l_outputSelected[l_destDirection] = true;
    //             }
    //         }
    //         else    {
    //             t_SwitchDirection l_destDirection = l_packet->f_getDestinationDirection();
    //             if(l_destDirection == E_PE)    {
    //                 if(!l_outputSelected[l_destDirection] && m_outputBuffer[l_destDirection]->f_hasCredit())  {
    //                     m_memInputBuffer->f_deallocate(l_packet);
    //                     m_outputBuffer[l_destDirection]->f_allocate(l_packet);
    //                     l_packet->f_setLastSwitchDirection(E_MM_IN);
    //                     l_transaction->f_logDebug(m_topology,"Switch Output Buffer directing to " + c_TypeToString::f_switchDirectionString(l_destDirection));
    //                     l_outputSelected[l_destDirection] = true;
    //                 }
    //             }
    //             else {
    //                 if(!l_outputSelectedMem && m_memOutputBuffer->f_hasCredit())  {
    //                     m_memInputBuffer->f_deallocate(l_packet);
    //                     m_memOutputBuffer->f_allocate(l_packet);
    //                     l_packet->f_setLastSwitchDirection(E_MM_IN);
    //                     l_transaction->f_logDebug(m_topology,"Switch Output Buffer directing to " + c_TypeToString::f_switchDirectionString(l_destDirection));
    //                     l_outputSelected[l_destDirection] = true;
    //                 }
    //             }
    //         }
    //     }
    // }
}

void c_Switch::f_execute() {
    for (unsigned l_dir = t_SwitchDirection::E_PE; l_dir < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_dir) {
        if (m_outputBuffer[l_dir] != NULL && !m_outputBuffer[l_dir]->f_empty())   {
            c_NetworkPacket *l_packet = (c_NetworkPacket *) m_outputBuffer[l_dir]->f_get(0);
            c_Transaction *l_transaction = l_packet->f_getTransaction();
            l_transaction->f_convertProtocolRequestToResponse();
            l_transaction->f_logDebug(m_topology,"Converting into response type");
        }
    }
    // if(m_isMemConnected && m_memOutputBuffer != NULL && !m_memOutputBuffer->f_empty())    {
    //     c_NetworkPacket *l_packet = (c_NetworkPacket *) m_memOutputBuffer->f_get(0);
    //     c_Transaction *l_transaction = l_packet->f_getTransaction();
    //     l_transaction->f_convertProtocolRequestToResponse();
    //     l_transaction->f_logDebug(m_topology,"Converting into response type");
    // }
}

void c_Switch::f_retire() {
    for (unsigned l_direction = t_SwitchDirection::E_PE; l_direction < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_direction) {
        m_masterPort[l_direction]->f_clockCycle();
    }
    // if(m_isMemConnected)    {
    //     m_memMasterPort->f_clockCycle();
    // }
}

c_Transaction* c_Switch::f_retireTransaction(t_PortNo p_portNo,  t_ResponseType p_respType) {
    if (p_portNo < E_MAX_DIR_SWITCH && m_outputBuffer[p_portNo] != NULL && !m_outputBuffer[p_portNo]->f_empty()) {
        c_NetworkPacket *l_packet = (c_NetworkPacket *) m_outputBuffer[p_portNo]->f_get(0);
        c_Transaction *l_transaction = l_packet->f_getTransaction();
        l_transaction->f_logDebug(m_topology,"Sending the packet out of the switch");
        m_outputBuffer[p_portNo]->f_deallocate(l_packet);
        return l_packet->f_getTransaction();
    }
    return NULL;
}

void c_Switch::f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction) {
    c_NetworkPacket *l_packet = p_transaction->f_getNetworkPacket();
    l_packet->f_setCurrentNode(m_nodeId);
    if(p_portNo < E_MAX_DIR_SWITCH) {
        p_transaction->f_logDebug(m_topology,"Getting the packet into the switch");
        m_inputBuffer[p_portNo]->f_allocate(l_packet);
    }
    else    {
        p_transaction->f_logDebug(m_topology,"Incorrect Direction");
    }
    p_transaction->f_logDebug(m_topology,"Switch Input Buffer");
}

void c_Switch::f_clockCycle() {
    f_watchDogTimer();
    f_retire();
    f_execute();
    f_issue();
    f_decode();
    f_fetch();
    m_clockCycle++;
}

bool c_Switch::f_getModuleDone() {
    bool l_isDone = true;
    for (unsigned l_direction = t_SwitchDirection::E_PE; l_direction < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_direction) {
        if (!m_inputBuffer[l_direction]->f_empty()) {
            l_isDone = false;
            break;
        }
        if (!m_outputBuffer[l_direction]->f_empty()) {
            l_isDone = false;
            break;
        }
    }
    return l_isDone;
}

void c_Switch::f_watchDogTimer() {
    for (unsigned l_direction = t_SwitchDirection::E_PE; l_direction < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_direction) {
        if (!m_inputBuffer[l_direction]->f_empty()) {
            for(t_Size l_index = 0; m_inputBuffer[l_direction] != NULL && l_index < m_inputBuffer[l_direction]->f_size(); l_index++) {
                if(m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_inputBuffer[l_direction]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_inputBuffer[l_direction]->f_get(l_index))->f_getTransaction()->f_logError(m_ctrl,"Recv Q Request Watchdog Timeout");
                }
            }
        }
        if (!m_outputBuffer[l_direction]->f_empty()) {
            for(t_Size l_index = 0; m_outputBuffer[l_direction] != NULL && l_index < m_outputBuffer[l_direction]->f_size(); l_index++) {
                if(m_ctrl->f_currentCycle() - ((c_NetworkPacket*) m_outputBuffer[l_direction]->f_get(l_index))->f_getOnRampTime() > m_topology->f_getSimConfig()->s_nocTimeout) {
                    ((c_NetworkPacket*) m_outputBuffer[l_direction]->f_get(l_index))->f_getTransaction()->f_logError(m_ctrl,"Send Q Request Watchdog Timeout");
                }
            }
        }
    }
}

std::string c_Switch::f_pipeString()   {
    std::string l_pipeString = "Switch[" + std::to_string(m_nodeId) + "]:";
    for (unsigned l_direction = t_SwitchDirection::E_PE; l_direction < t_SwitchDirection::E_MAX_DIR_SWITCH; ++l_direction) {
        l_pipeString = l_pipeString + c_TypeToString::f_switchDirectionString((t_SwitchDirection)l_direction) + "(IB: ";
        if (!m_inputBuffer[l_direction]->f_empty()) {
            for(t_Size l_index = 0; m_inputBuffer[l_direction] != NULL && l_index < m_inputBuffer[l_direction]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_inputBuffer[l_direction]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
        }
        l_pipeString = l_pipeString + ")(OB: ";
        if (!m_outputBuffer[l_direction]->f_empty()) {
            for(t_Size l_index = 0; m_outputBuffer[l_direction] != NULL && l_index < m_outputBuffer[l_direction]->f_size(); l_index++) {
                t_Counter l_transId = ((c_NetworkPacket*) m_outputBuffer[l_direction]->f_get(l_index))->f_getTransaction()->f_getTransactionId();
                l_pipeString = l_pipeString + std::to_string(l_transId) + ",";
            }
        }
        l_pipeString = l_pipeString + ")";
    }
    return l_pipeString;
}

bool c_Switch::f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType)   {
    if(p_portNo < E_MAX_DIR_SWITCH) {
        return m_inputBuffer[p_portNo]->f_hasCredit();
    }
    else    {
        assert(false);
    }
}

bool c_Switch::isCurrentNodeDestination(c_NetworkPacket *p_packet) {
    return p_packet->f_getDestination() == m_nodeId;
}

c_MasterPort* c_Switch::f_getMasterPort(t_SwitchDirection p_direction)    {
    return m_masterPort[p_direction];
}

c_SlavePort* c_Switch::f_getSlavePort(t_SwitchDirection p_direction)  {
    return m_slavePort[p_direction];
}

void c_Switch::f_portCheck()  {
    for(unsigned l_dir = E_PE; l_dir < E_MAX_DIR_SWITCH; l_dir++)   {
        if(m_nodeIdNeighbours[l_dir] != -1) {
                    m_masterPort[l_dir]->f_portCheck();
                    m_slavePort[l_dir]->f_portCheck();
        }
    }
}

t_RepeaterDirection c_Switch::f_oppositeDirection(c_Node *p_Node, t_NodeId p_NodeId){
    for (unsigned l_dir = t_RepeaterDirection::E_PREV; l_dir < t_RepeaterDirection::E_MAX_DIR_REPEATER; ++l_dir){
        if(p_Node->f_getNeighbour()[l_dir] == p_NodeId) {
            return (t_RepeaterDirection) l_dir;
        }
    }

    return t_RepeaterDirection::E_MAX_DIR_REPEATER;
}
