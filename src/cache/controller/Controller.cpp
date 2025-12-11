#include "Controller.hpp"

c_Controller::c_Controller(std::string p_name)	{
    m_name = p_name;
}

std::string c_Controller::f_getName()   {
    return m_name;
}