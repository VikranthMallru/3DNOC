#include "Statistics.hpp"
#include <iostream>
#include <assert.h>

t_Size c_Statistics::m_numCaches = 0;
std::string c_Statistics::m_statsFileName = "";
std::ofstream c_Statistics::m_statsFile;

std::vector<std::vector<std::vector<t_Counter>>> c_Statistics::m_hits;
std::vector<std::vector<std::vector<t_Counter>>> c_Statistics::m_hitsNoPerm;
std::vector<std::vector<std::vector<t_Counter>>> c_Statistics::m_replaced;
std::vector<std::vector<t_Counter>> c_Statistics::m_misses;
std::vector<std::vector<t_Counter>> c_Statistics::m_requests;
std::vector<std::vector<t_Counter>> c_Statistics::m_recvQOccupancy;
std::vector<std::vector<t_Counter>> c_Statistics::m_sendQOccupancy;
std::vector<std::vector<t_Counter>> c_Statistics::m_recvQDelay;


c_Statistics::c_Statistics(t_Size p_numCaches, std::string p_statsFileName)   {
    m_numCaches = p_numCaches;
    m_statsFileName = p_statsFileName;
    m_statsFile.open(m_statsFileName,std::fstream::out);
    m_requests.resize(m_numCaches);
    m_misses.resize(m_numCaches);
    m_recvQOccupancy.resize(m_numCaches);
    m_recvQDelay.resize(m_numCaches);
    m_sendQOccupancy.resize(m_numCaches);
    m_hits.resize(m_numCaches);
    m_hitsNoPerm.resize(m_numCaches);
    m_replaced.resize(m_numCaches);
    for(t_Size l_cacheNum = 0; l_cacheNum < m_numCaches; l_cacheNum++)   {
        m_requests[l_cacheNum].resize((t_Size)t_RequestType::E_NUM_RECVQ_TYPE + 1);
        m_hits[l_cacheNum].resize((t_Size)t_RequestType::E_NUM_RECVQ_TYPE + 1);
        m_hitsNoPerm[l_cacheNum].resize((t_Size)t_RequestType::E_NUM_RECVQ_TYPE + 1);
        m_replaced[l_cacheNum].resize((t_Size)t_RequestType::E_NUM_RECVQ_TYPE + 1);
        m_misses[l_cacheNum].resize((t_Size)t_RequestType::E_NUM_RECVQ_TYPE + 1);
        m_recvQOccupancy[l_cacheNum].resize((t_Size)t_RequestType::E_NUM_RECVQ_TYPE + 1);
        m_recvQDelay[l_cacheNum].resize((t_Size)t_RequestType::E_NUM_RECVQ_TYPE + 1);
        m_sendQOccupancy[l_cacheNum].resize((t_Size)t_ResponseType::E_NUM_SENDQ_TYPE + 1);
        for(t_Size l_reqType = 0; l_reqType <= (t_Size)t_RequestType::E_NUM_RECVQ_TYPE; l_reqType++)   {
            m_requests[l_cacheNum][l_reqType] = 0;
            m_misses[l_cacheNum][l_reqType] = 0;
            m_recvQOccupancy[l_cacheNum][l_reqType] = 0;
            m_recvQDelay[l_cacheNum][l_reqType] = 0;
            m_hits[l_cacheNum][l_reqType].resize((t_Size)t_CacheState::E_NUM_STATES + 1);
            m_hitsNoPerm[l_cacheNum][l_reqType].resize((t_Size)t_CacheState::E_NUM_STATES + 1);
            m_replaced[l_cacheNum][l_reqType].resize((t_Size)t_CacheState::E_NUM_STATES + 1);
            for(t_Size l_state = 0; l_state <= (t_Size)t_CacheState::E_NUM_STATES; l_state++)   {
                m_hits[l_cacheNum][l_reqType][l_state] = 0;
                m_hitsNoPerm[l_cacheNum][l_reqType][l_state] = 0;
                m_replaced[l_cacheNum][l_reqType][l_state] = 0;
            }
        }
        for(t_Size l_respType = 0; l_respType <= (t_Size)t_ResponseType::E_NUM_SENDQ_TYPE; l_respType++)   {
            m_sendQOccupancy[l_cacheNum][l_respType] = 0;
        }
    }
}

c_Statistics::~c_Statistics()    {
    try {
        m_statsFile.close();
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error Closing File';
    }
}

void c_Statistics::f_displayStats()    {
    try {
        for(t_Size l_count = 0; l_count < m_numCaches; l_count++)   {
            std::string l_stats = "Cache-" + std::to_string(l_count) + "\n";
            for(t_Size l_reqType = 0; l_reqType <= (t_Size)t_RequestType::E_NUM_RECVQ_TYPE; l_reqType++)   {
                l_stats = (l_stats + std::to_string(c_Statistics::m_requests[l_count][l_reqType]) + "\t");
                l_stats = (l_stats + std::to_string(c_Statistics::m_misses[l_count][l_reqType]) + "\t");
                l_stats = (l_stats + std::to_string(c_Statistics::m_recvQOccupancy[l_count][l_reqType]) + "\t");
                l_stats = (l_stats + std::to_string(c_Statistics::m_recvQDelay[l_count][l_reqType]) + "\t");
                for(t_Size l_state = 0; l_state <= (t_Size)t_CacheState::E_NUM_STATES; l_state++)   {
                    l_stats = (l_stats + std::to_string(c_Statistics::m_hits[l_count][l_reqType][l_state]) + "\t");
                    l_stats = (l_stats + std::to_string(c_Statistics::m_replaced[l_count][l_reqType][l_state]) + "\t");
                }
            }
            for(t_Size l_respType = 0; l_respType <= (t_Size)t_ResponseType::E_NUM_SENDQ_TYPE; l_respType++)   {
                l_stats = (l_stats + std::to_string(c_Statistics::m_sendQOccupancy[l_count][l_respType]) + "\t");
            }
            l_stats += "\n";
            m_statsFile<<l_stats.c_str();
        }
    }
    catch(const std::exception& e)  {
        std::cerr << 'Error writing stats\n';
    }
}
