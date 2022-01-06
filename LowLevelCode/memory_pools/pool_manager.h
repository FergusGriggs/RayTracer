#pragma once

#include <unordered_map>

#include "memory_pool.h"

class PoolManager
{
public:
    ~PoolManager();

    static PoolManager& it();

    void  initialise();

    void* allocateMemory(size_t size);
    void  deallocateMemory(size_t size, void* header);

    void clearPools();

    bool getDontUsePoolsOverride() const;
    void setDontUsePoolsOverride(bool dontUsePoolsOverride);

private:
    PoolManager();

private:
    std::unordered_map<size_t, MemoryPool*>* m_pools;
    bool m_dontUsePoolsOverride;
};

