#pragma once

struct PoolChunk
{
    PoolChunk();

    PoolChunk* m_next;
};

struct BlockInfo
{
    BlockInfo();
    ~BlockInfo();

    void*  m_mallocAddress;
    BlockInfo* m_next;
};

class MemoryPool
{
public:
    MemoryPool(size_t size);
    ~MemoryPool();

    void* allocateMemory();
    void  decallocateMemory(void* header);

    void addMemoryBlock();
    void deleteAllAllocatedBlocks();

private:
    PoolChunk* m_firstFreePoolChunk;
    BlockInfo* m_newestBlockInfo;

    size_t m_size;
    int    m_chunksAllocated;
    int    m_currentBlockSize;
};
