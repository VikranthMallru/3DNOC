#ifndef SWITCH_HPP
#define SWITCH_HPP

#include <vector>
#include <queue>
#include "types.hpp"
#include "Node.hpp"
#include "Coordinate.hpp"
#include "NetworkPacket.hpp"
#include "Transaction.hpp"


class c_TopologyConfig;
class c_Topology;

class c_Switch : public c_Node {
private:
    t_SwitchDirection m_lastDirection;
public:
    c_Switch(c_Topology *p_topology, t_NodeId p_nodeId, c_Coordinate *p_coordinate, 
            t_NodeId *p_nodeIdNeighbours, t_Credit *p_bufferSize);
    ~c_Switch();

    static t_SwitchDirection f_oppositeDirection(t_SwitchDirection p_direction);
    static t_RepeaterDirection f_oppositeDirection(c_Node *p_Node, t_NodeId p_NodeId);

    void f_fetch();
    void f_decode();
    void f_issue();
    void f_execute();
    void f_retire();

    void f_clockCycle();

    c_Transaction* f_retireTransaction(t_PortNo p_portNo,  t_ResponseType p_respType);
	void f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction);
	bool f_getModuleDone();
	bool f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType);
	void f_watchDogTimer();

    bool isCurrentNodeDestination(c_NetworkPacket *p_packet);
    c_MasterPort* f_getMasterPort(t_SwitchDirection p_direction);
    c_SlavePort* f_getSlavePort(t_SwitchDirection p_direction);
    void f_portCheck();
    std::string f_pipeString();
};

#endif
