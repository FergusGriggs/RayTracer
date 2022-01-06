#include "pool_manager.h"

#include "../header_footer.h"

#include <iostream>

PoolManager::~PoolManager()
{
}

PoolManager& PoolManager::it()
{
    static PoolManager s;
    return s;
}

void PoolManager::initialise()
{
    std::cout << "Pool manager initialised\n";

    m_pools = new std::unordered_map<size_t, MemoryPool*>();
}

void* PoolManager::allocateMemory(size_t size)
{
    void* memoryAddress = nullptr;

    auto itr = m_pools->find(size);
    if (itr == m_pools->end())
    {
        m_dontUsePoolsOverride = true;

        MemoryPool* newPool = new MemoryPool(size);
        m_pools->insert({ size, newPool });

        m_dontUsePoolsOverride = false;

        memoryAddress = newPool->allocateMemory();
    }
    else
    {
        memoryAddress = itr->second->allocateMemory();
    }

    return memoryAddress;
}

void PoolManager::deallocateMemory(size_t size, void* header)
{
    auto itr = m_pools->find(size);
    if (itr != m_pools->end())
    {
        itr->second->decallocateMemory(header);
    }
    else
    {
        MEM_ALLOC_COUT_ERROR("Trying to deallocate memory that was not pool allocated")
    }
}

void PoolManager::clearPools()
{
    for (auto& pool : (*m_pools))
    {
        delete pool.second;
    }
    m_pools->clear();

    delete m_pools;

    std::cout << "Cleared memory pools\n";
}

bool PoolManager::getDontUsePoolsOverride() const
{
    return m_dontUsePoolsOverride;
}

void PoolManager::setDontUsePoolsOverride(bool dontUsePoolsOverride)
{
    m_dontUsePoolsOverride = dontUsePoolsOverride;
}

PoolManager::PoolManager() :
    m_pools(nullptr),
    m_dontUsePoolsOverride(true)
{
}
