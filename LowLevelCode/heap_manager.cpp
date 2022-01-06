#include "heap_manager.h"

#include "heap_types/audio_heap.h"
#include "heap_types/graphics_heap.h"
#include "heap_types/physics_heap.h"

#include <iostream>

HeapManager::HeapManager() :
    m_defaultHeap(),
    m_numHeaps(0)
{
    addHeapPtr(&m_defaultHeap);
}

HeapManager::~HeapManager()
{
    printHeapUsages();

    // Dont delete default heap as it's stack allocated
    for (int heapIndex = 1; heapIndex < m_numHeaps; ++heapIndex)
    {
        if (m_allHeaps[heapIndex] != nullptr)
        {
            delete m_allHeaps[heapIndex];
        }
    }
}

void HeapManager::initialiseSpecificHeaps()
{
    addHeapPtr(new AudioHeap());
    addHeapPtr(new GraphicsHeap());
    addHeapPtr(new PhysicsHeap());
}

void HeapManager::addHeapPtr(ManagedHeap* heap)
{
    if (heap == nullptr) return;

    m_allHeaps[(int)heap->getType()] = heap;

    ++m_numHeaps;
}

DefaultHeap* HeapManager::getDefaultHeap()
{
    return &m_defaultHeap;
}

void HeapManager::printHeapUsages() const
{
    std::cout << "\nBytes allocated for heaps:\n";
    for (int heapIndex = 0; heapIndex < m_numHeaps; ++heapIndex)
    {
        std::cout << "    " << m_allHeaps[heapIndex]->getTypeName() << ": " << m_allHeaps[heapIndex]->getBytesAllocated() << "\n";
        m_allHeaps[heapIndex]->checkHeapIntegrity();
    }
    std::cout << "\n";

}

ManagedHeap* HeapManager::getHeapPtr(ManagedHeap::Type heapType)
{
    return m_allHeaps[(int)heapType];
}
