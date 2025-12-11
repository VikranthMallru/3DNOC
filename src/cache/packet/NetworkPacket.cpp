#include "NetworkPacket.hpp"
#include "Transaction.hpp"
#include "TypeToString.hpp"
#include <assert.h>

t_PacketId c_NetworkPacket::m_packetIdGenerator = 0;

c_NetworkPacket::c_NetworkPacket() {}

c_NetworkPacket::~c_NetworkPacket() {}

void c_NetworkPacket::f_initPacket(t_NetworkChannel p_channel, t_NodeId p_source, t_NodeId p_destination, c_Transaction *p_transaction, 
        t_SwitchDirection p_sourceDirection, t_Time p_currTime)    {
    m_channel = p_channel;
    m_source = p_source;
    m_currentNode = m_source;
    m_destination = p_destination;
    m_transaction = p_transaction;
    m_sourceDirection = p_sourceDirection;
    m_destinationDirection = E_MAX_DIR_SWITCH;
    m_lastSwitchDirection = p_sourceDirection;
    m_lastRepeaterDirection = t_RepeaterDirection::E_PREV;
    m_onRampTime = p_currTime;
    m_offRampTime = 0;
    m_packetId = m_packetIdGenerator++;
}

void c_NetworkPacket::f_copyPacket(c_NetworkPacket *p_packet, t_Time p_currTime)    {
    m_channel = p_packet->f_getChannel();
    m_source = p_packet->f_getSource();
    m_currentNode = m_source;
    m_destination = p_packet->f_getDestination();
    m_transaction = NULL;
    m_sourceDirection = p_packet->f_getSourceDirection();
    m_destinationDirection = E_MAX_DIR_SWITCH;
    m_lastSwitchDirection = m_sourceDirection;
    m_lastRepeaterDirection = t_RepeaterDirection::E_PREV;
    m_onRampTime = p_currTime;
    m_offRampTime = 0;
    m_packetId = m_packetIdGenerator++;
}

t_NodeId c_NetworkPacket::f_getSource() {
    return m_source;
}

void c_NetworkPacket::f_setSource(t_NodeId p_source) {
    m_source = p_source;
}

t_NetworkChannel c_NetworkPacket::f_getChannel() {
    return m_channel;
}

void c_NetworkPacket::f_setChannel(t_NetworkChannel p_channel) {
    m_channel = p_channel;
}

t_NodeId c_NetworkPacket::f_getDestination() {
    return m_destination;
}

void c_NetworkPacket::f_setDestination(t_NodeId p_destination) {
    m_destination = p_destination;
}

t_SwitchDirection c_NetworkPacket::f_getLastSwitchDirection() {
    return m_lastSwitchDirection;
}

void c_NetworkPacket::f_setLastSwitchDirection(t_SwitchDirection p_lastSwitchDirection) {
    m_lastSwitchDirection = p_lastSwitchDirection;
}


t_RepeaterDirection c_NetworkPacket::f_getLastRepeaterDirection() {
    return m_lastRepeaterDirection;
}

void c_NetworkPacket::f_setLastRepeaterDirection(t_RepeaterDirection p_lastRepeaterDirection) {
    m_lastRepeaterDirection = p_lastRepeaterDirection;
}

t_PacketId c_NetworkPacket::f_getPacketId() {
    return m_packetId;
}

c_Transaction* c_NetworkPacket::f_getTransaction() {
    return m_transaction;
}

void c_NetworkPacket::f_setTransaction(c_Transaction *p_transaction) {
    m_transaction = p_transaction;
}

std::string c_NetworkPacket::f_toString(c_Controller *p_ctrl,std::string p_message)  {
    if(m_transaction == NULL)   {
        std::cout<<"PacketID:"<<std::to_string(m_packetId)<<std::endl;
        assert(false);
    }  
    return (std::to_string(m_channel) + "\t" + std::to_string(m_source) + "\t" + std::to_string(m_destination)
     + "\t" + c_TypeToString::f_switchDirectionString(m_sourceDirection) + "\t" + c_TypeToString::f_switchDirectionString(m_destinationDirection)
     + "\t" + std::to_string(m_currentNode) + "\t" + c_TypeToString::f_switchDirectionString(m_lastSwitchDirection) + "\t" + c_TypeToString::f_repeaterDirectionString(m_lastRepeaterDirection)
     + "\t" + std::to_string(m_onRampTime) + "\t" + std::to_string(m_offRampTime)); 
}
    
void c_NetworkPacket::f_reset()  {
    m_transaction = NULL;
    m_packetId = -1;
    m_channel = M_MAX_NODE;
    m_source = M_MAX_NODE;
    m_destination = M_MAX_NODE;
    m_sourceDirection = E_MAX_DIR_SWITCH;
    m_destinationDirection = E_MAX_DIR_SWITCH;
    m_currentNode = m_source;
    m_lastSwitchDirection = t_SwitchDirection::E_PE;
    m_lastRepeaterDirection = t_RepeaterDirection::E_PREV;
    m_onRampTime = 0;
    m_offRampTime = 0;
}

t_NodeId c_NetworkPacket::f_getCurrentNode()    {
    return m_currentNode;
}
    
void c_NetworkPacket::f_setCurrentNode(t_NodeId p_currentNode)  {
    m_currentNode = p_currentNode;
}

t_Time c_NetworkPacket::f_getOnRampTime()  {
    return m_onRampTime;
}

void c_NetworkPacket::f_setOnRampTime(t_Time p_time)   {
    m_onRampTime = p_time;
}

t_Time c_NetworkPacket::f_getOffRampTime() {
    return m_offRampTime;
}

void c_NetworkPacket::f_setOffRampTime(t_Time p_time)  {
    m_offRampTime = p_time;
}

void c_NetworkPacket::f_findDestinationDirection(t_RequestType p_reqType)  {
    switch(p_reqType)    {
    case E_CORE_READ:
    case E_CORE_WRITE:
    case E_PROBE_RESP:
    case E_PREFETCH_RECV:
        if(m_sourceDirection == E_PE)
            m_destinationDirection = E_MM;
        else
            assert(false);
        break;
    case E_PROBE_REQ:
        if(m_sourceDirection == E_PE)
            m_destinationDirection = E_MM;
        else if(m_sourceDirection == E_MM)
            m_destinationDirection = E_PE;
        else
            assert(false);
        break;
    case E_FILL:
        if(m_sourceDirection == E_MM)
            m_destinationDirection = E_PE;//m_destinationDirection = E_CACHE;
        else
            assert(false);
        break;
    default:
        assert(false);
    }
}


void c_NetworkPacket::f_findDestinationDirectionForNoCache(t_RequestType p_reqType)  {
    switch(p_reqType)    {
    case E_CORE_READ:
    case E_CORE_WRITE:
    case E_PROBE_RESP:
    case E_PREFETCH_RECV:
        if(m_sourceDirection == E_PE)
            m_destinationDirection = E_MM;
        else
            assert(false);
        break;
    case E_PROBE_REQ:
        if(m_sourceDirection == E_MM)
            m_destinationDirection = E_PE;
        else
            assert(false);
        break;
    case E_FILL:
        if(m_sourceDirection == E_MM)
            m_destinationDirection = E_PE;
        else
            assert(false);
        break;
    default:
        assert(false);
    }
}

t_SwitchDirection c_NetworkPacket::f_getSourceDirection()    {
    return m_sourceDirection;
}

void c_NetworkPacket::f_setSourceDirection(t_SwitchDirection p_sourceDirection)   {
    m_sourceDirection = p_sourceDirection;
}

t_SwitchDirection c_NetworkPacket::f_getDestinationDirection()   {
    return m_destinationDirection;
}

void c_NetworkPacket::f_setDestinationDirection(t_SwitchDirection p_destinationDirection) {
    m_destinationDirection = p_destinationDirection;
}
