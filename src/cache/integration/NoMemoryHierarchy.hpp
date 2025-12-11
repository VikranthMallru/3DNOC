#include "types.hpp"
#include "SimConfig.hpp"
#include "MemoryHierarchy.hpp"
#include <string>

#ifndef NOMEMORYHIERARCHY_H
#define NOMEMORYHIERARCHY_H

class c_NoMemoryHierarchy  : public c_MemoryHierarchy {
public:
    c_NoMemoryHierarchy(c_CacheSimulator *p_cacheSim,c_SimConfig *p_simConfig);
    ~c_NoMemoryHierarchy();
    void f_createCaches();
    void f_linkCoreAndCache();
    void f_createMultiCacheNetwrokInterface();
    void f_linkPrivateCaches();
    void f_linkCacheInterconnect();
    void f_linkCoreMainMemory();
    void f_linkCoreInterconnect();
    void f_createHierarchy();
    void f_resetClock();
    void f_portCheck();
};

#endif
