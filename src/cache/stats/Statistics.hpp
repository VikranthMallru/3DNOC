#include "types.hpp"
#include <fstream>
#include <vector>


#ifndef STATISTICS_H
#define STATISTICS_H

class c_Statistics  {
public:
    static std::vector<std::vector<std::vector<t_Counter>>> m_hits;
    static std::vector<std::vector<std::vector<t_Counter>>> m_hitsNoPerm;
    static std::vector<std::vector<std::vector<t_Counter>>> m_replaced;
    static std::vector<std::vector<t_Counter>> m_misses;
    static std::vector<std::vector<t_Counter>> m_requests;
    
    static std::vector<std::vector<t_Counter>> m_recvQOccupancy;
    static std::vector<std::vector<t_Counter>> m_sendQOccupancy;
    static std::vector<std::vector<t_Counter>> m_recvQDelay;

    static t_Size m_numCaches;
    static std::string m_statsFileName;
    static std::ofstream m_statsFile;

    c_Statistics(t_Size p_numCaches, std::string p_statsFileName);
    ~c_Statistics();
    static void f_displayStats();
};

#endif