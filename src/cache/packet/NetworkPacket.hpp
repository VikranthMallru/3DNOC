#ifndef NETWORK_PACKET_HPP
#define NETWORK_PACKET_HPP
#include "types.hpp"
#include "Request.hpp"
#include "CacheLogger.hpp"

class c_Transaction;

class c_NetworkPacket : public c_Request {
private:
    static t_PacketId m_packetIdGenerator;
    t_PacketId m_packetId;
    t_NodeId m_source;
    t_NodeId m_destination;
    t_NetworkChannel m_channel;
    t_SwitchDirection m_lastSwitchDirection;
    t_RepeaterDirection m_lastRepeaterDirection;
    t_Time m_onRampTime;
    t_Time m_offRampTime;
    c_Transaction *m_transaction;
    t_NodeId m_currentNode;
    t_SwitchDirection m_sourceDirection;
    t_SwitchDirection m_destinationDirection;
public:
    t_NodeId f_getSource();
    void f_setSource(t_NodeId p_source);
    t_NodeId f_getDestination();
    void f_setDestination(t_NodeId p_destination);
    t_SwitchDirection f_getSourceDirection();
    void f_setSourceDirection(t_SwitchDirection p_source);
    t_SwitchDirection f_getDestinationDirection();
    void f_setDestinationDirection(t_SwitchDirection p_destination);
    void f_setLastRepeaterDirection(t_RepeaterDirection p_lastRepeaterDirection);
    void f_setLastSwitchDirection(t_SwitchDirection p_lastSwitchDirection);
    t_RepeaterDirection f_getLastRepeaterDirection();
    t_SwitchDirection f_getLastSwitchDirection();
    void f_findDestinationDirection(t_RequestType p_reqType);
    void f_findDestinationDirectionForNoCache(t_RequestType p_reqType);

    t_NodeId f_getCurrentNode();
    void f_setCurrentNode(t_NodeId p_currentNode);

    t_PacketId f_getPacketId();
    c_NetworkPacket();
    ~c_NetworkPacket();
    void f_initPacket(t_NetworkChannel p_channel, t_NodeId p_source, t_NodeId p_destination, c_Transaction *p_transaction, 
        t_SwitchDirection p_currDirection, t_Time p_currTime);
    t_NetworkChannel f_getChannel();
    void f_setChannel(t_NetworkChannel p_channel);

    t_Time f_getOnRampTime();
    void f_setOnRampTime(t_Time p_time);
    t_Time f_getOffRampTime();
    void f_setOffRampTime(t_Time p_time);



    c_Transaction *f_getTransaction();
    void f_setTransaction(c_Transaction *p_transaction);
    void f_copyPacket(c_NetworkPacket *p_packet, t_Time p_currTime);

    std::string f_toString(c_Controller *p_ctrl,std::string p_message);
    void f_reset();
};

#endif