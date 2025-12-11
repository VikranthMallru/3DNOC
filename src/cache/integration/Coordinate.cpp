#include "Coordinate.hpp"

c_Coordinate::c_Coordinate(t_NetworkChannel p_channel,t_NodeId p_W,t_NodeId p_Z,
                        t_NodeId p_Y,t_NodeId p_X)    {
    m_channel = p_channel;
    m_W = p_W;
    m_Z = p_Z;
    m_Y = p_Y;
    m_X = p_X;
}

t_NetworkChannel c_Coordinate::f_getChannel() {
    return m_channel;
}

t_NodeId c_Coordinate::f_getW()    {
    return m_W;
}

t_NodeId c_Coordinate::f_getZ()    {
    return m_Z;
}

t_NodeId c_Coordinate::f_getY()    {
    return m_Y;
}

t_NodeId c_Coordinate::f_getX()    {
    return m_X;
}

std::string c_Coordinate::f_getString()   {
    return std::to_string((unsigned)m_channel) + "-" + std::to_string((unsigned)m_X) + "-" +
             std::to_string((unsigned)m_Y) + "-" + std::to_string((unsigned)m_Z) + "-" + std::to_string((unsigned)m_W);
}