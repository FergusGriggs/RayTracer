#include "memory_pool.h"

#include <stdlib.h>

#define INITIAL_BLOCK_SIZE 4

PoolChunk::PoolChunk() :
    m_next(nullptr)
{
}

BlockInfo::BlockInfo() :
    m_mallocAddress(nullptr),
    m_next(nullptr)
{
}

BlockInfo::~BlockInfo()
{
    if (m_mallocAddress != nullptr)
    {
        free(m_mallocAddress);
    }

    if (m_next != nullptr)
    {
        m_next->~BlockInfo();
        free(m_next);
    }
}

MemoryPool::MemoryPool(size_t size) :
    m_firstFreePoolChunk(nullptr),
    m_newestBlockInfo(nullptr),

    m_size(size),
    m_chunksAllocated(0),
    m_currentBlockSize(INITIAL_BLOCK_SIZE)
{
}

MemoryPool::~MemoryPool()
{
}

void* MemoryPool::allocateMemory()
{
    if (m_firstFreePoolChunk == nullptr)
    {
        addMemoryBlock();
        return allocateMemory();
    }

    ++m_chunksAllocated;

    void* retValue = m_firstFreePoolChunk;
    m_firstFreePoolChunk = m_firstFreePoolChunk->m_next;

    return retValue;
}

void MemoryPool::decallocateMemory(void* header)
{
    --m_chunksAllocated;

    PoolChunk* poolChunk = reinterpret_cast<PoolChunk*>(header);

    poolChunk->m_next = m_firstFreePoolChunk;

    m_firstFreePoolChunk = poolChunk;
}

void MemoryPool::addMemoryBlock()
{
    void* memoryBlock = malloc(m_size * m_currentBlockSize);
    
    if (memoryBlock != nullptr)
    {
        // Set the first free chunk
        PoolChunk* chunk = reinterpret_cast<PoolChunk*>(memoryBlock);
        m_firstFreePoolChunk = chunk;

        // Link it up to the other newly created block chunks
        for (int chunkIndex = 0; chunkIndex < m_currentBlockSize - 1; ++chunkIndex)
        {
            char* chunkChar = reinterpret_cast<char*>(chunk);
            if (chunkChar != nullptr)
            {
                chunkChar += m_size;
                chunk->m_next = reinterpret_cast<PoolChunk*>(chunkChar);
                chunk = chunk->m_next;
            }
        }

        // Make sure the last chunk does not point anywhere dodgy
        if (chunk != nullptr)
        {
            chunk->m_next = nullptr;
        }

        // Increase the block size exponentially
        m_currentBlockSize *= 1.5;

        // Add block info so we know where to delete later
        BlockInfo* blockInfo = reinterpret_cast<BlockInfo*>(malloc(sizeof(BlockInfo)));
        if (blockInfo)
        {
            blockInfo->m_next = m_newestBlockInfo;
            blockInfo->m_mallocAddress = memoryBlock;
            m_newestBlockInfo = blockInfo;
        }     
    }
}

void MemoryPool::deleteAllAllocatedBlocks()
{
    if (m_newestBlockInfo != nullptr)
    {
        m_newestBlockInfo->~BlockInfo();
    }
}
