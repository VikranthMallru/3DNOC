#ifndef COORDINATE_HPP
#define COORDINATE_HPP
#include "types.hpp"
#include <string>

class c_Coordinate  {
private:
    t_NetworkChannel m_channel;
    t_NodeId m_W;
    t_NodeId m_Z;
    t_NodeId m_Y;
    t_NodeId m_X;
public:
    t_NetworkChannel f_getChannel();
    t_NodeId f_getW();
    t_NodeId f_getZ();
    t_NodeId f_getY();
    t_NodeId f_getX();
    c_Coordinate(t_NetworkChannel p_channel,t_NodeId p_W,t_NodeId p_Z,
                        t_NodeId p_Y,t_NodeId p_X);
    std::string f_getString();
};
#endif