#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <queue>
#include "types.hpp"
#include "NetworkPacket.hpp"
#include "Transaction.hpp"
#include "MasterPort.hpp"
#include "SlavePort.hpp"
#include "Queue.hpp"
#include "Coordinate.hpp"
#include "Controller.hpp"

class c_TopologyConfig;
class c_Topology;

class c_Node : public c_Controller {
protected:
    t_NodeId m_nodeId;
    t_NodeId *m_nodeIdNeighbours;
    t_Credit *m_bufferSize;
    c_Coordinate *m_coordinate;
    c_MasterPort **m_masterPort;
    c_SlavePort **m_slavePort;
    c_Queue **m_inputBuffer;
    c_Queue **m_outputBuffer;
    t_Time m_clockCycle;
    c_Topology *m_topology;
    t_DeviceType m_deviceType;
public:
    c_Node(c_Topology *p_topology, t_NodeId p_nodeId, c_Coordinate *p_coordinate, 
            t_NodeId *p_nodeIdNeighbours, t_Credit *p_bufferSize, t_DeviceType p_deviceType);
    virtual ~c_Node();
    //virtual void f_bind(c_Node *p_allNodes) = 0;
    t_DeviceType f_getType();
    t_NodeId f_getNodeId();
    c_Coordinate *f_getCoordinate();
    c_MasterPort *f_getMasterPort(unsigned p_dir);
    c_SlavePort *f_getSlavePort(unsigned p_dir);
    virtual void f_fetch() = 0;
    virtual void f_decode() = 0;
    virtual void f_issue() = 0;
    virtual void f_execute() = 0;
    virtual void f_retire() = 0;
    t_Time f_currentCycle();
    void f_resetClock();
    virtual void f_clockCycle() = 0;
    virtual c_Transaction* f_retireTransaction(t_PortNo p_portNo,  t_ResponseType p_respType) = 0;
	virtual void f_fetchTransaction(t_PortNo p_portNo, c_Transaction *p_transaction) = 0;
	virtual bool f_getModuleDone() = 0;
	virtual bool f_hasCreditRecvQueue(t_PortNo p_portNo, t_RequestType p_nextReqType) = 0;
	virtual void f_watchDogTimer() = 0;
    t_NodeId* f_getNeighbour();
    std::string f_toString();
    virtual void f_portCheck() = 0;
    virtual std::string f_pipeString() = 0; 
};

#endif
