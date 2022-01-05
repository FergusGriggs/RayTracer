#include "heap_manager.h"

#include "heap_types/audio_heap.h"
#include "heap_types/graphics_heap.h"
#include "heap_types/physics_heap.h"

#include <iostream>

HeapManager::HeapManager() :
    m_heaps()
{
    addHeapPtr(new AudioHeap());
    addHeapPtr(new GraphicsHeap());
    addHeapPtr(new PhysicsHeap());
}

HeapManager::~HeapManager()
{
    for (auto& heap : m_heaps)
    {
        delete heap.second;
    }

    std::cout << "Default heap bytes allocated at end of program " << HeapManager::getHeapPtr(ManagedHeap::Type::eDefault)->getBytesAllocated() << "\n";
}

void HeapManager::addHeapPtr(ManagedHeap* heap)
{
    if (heap == nullptr) return;
    if (heap->getType() == ManagedHeap::Type::eDefault) return;

    m_heaps[heap->getType()] = heap;
}

ManagedHeap* HeapManager::getHeapPtr(ManagedHeap::Type heapType)
{
    if (heapType == ManagedHeap::Type::eDefault)
    {
        return &s_defaultHeap;
    }

    return m_heaps[heapType];
    //return (*m_heaps)[heapType];
}
