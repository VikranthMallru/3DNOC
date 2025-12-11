#include "Node.hpp"
#include "Coordinate.hpp"
#include "TopologyConfig.hpp"
#include "Topology.hpp"
#include "TypeToString.hpp"

c_Node::c_Node(c_Topology *p_topology, t_NodeId p_nodeId, c_Coordinate *p_coordinate, 
            t_NodeId *p_nodeIdNeighbours, t_Credit *p_bufferSize, t_DeviceType p_deviceType) : c_Controller("") {
    m_topology = p_topology;
    m_nodeId = p_nodeId;
    m_clockCycle = 0;
    m_coordinate = p_coordinate;
    m_nodeIdNeighbours = p_nodeIdNeighbours;
    m_bufferSize = p_bufferSize;
    m_deviceType = p_deviceType;
}

c_Node::~c_Node() {
    delete m_coordinate;
    delete[] m_bufferSize;
    delete[] m_nodeIdNeighbours;
}

t_DeviceType c_Node::f_getType()    {
    return m_deviceType;
}

c_MasterPort *c_Node::f_getMasterPort(unsigned p_dir)    {
    return m_masterPort[p_dir];
}

c_SlavePort *c_Node::f_getSlavePort(unsigned p_dir)  {
    return m_slavePort[p_dir];
}

t_NodeId* c_Node::f_getNeighbour()  {
    return m_nodeIdNeighbours;
}


c_Coordinate *c_Node::f_getCoordinate() {
    return m_coordinate;
}

t_NodeId c_Node::f_getNodeId() {
    return m_nodeId;
}

t_Time c_Node::f_currentCycle() {
    return m_clockCycle;
}

void c_Node::f_resetClock() {
    m_clockCycle = 0;
}

std::string c_Node::f_toString()    {
    return "Node: " + std::to_string(m_nodeId) + " Type: " + c_TypeToString::f_deviceTypeString(m_deviceType) + "\n";
}