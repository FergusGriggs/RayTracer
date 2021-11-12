#include "GraphicsObject.h"

#include "../HeapManager.h"
#include "../GlobalAllocation.h"

GraphicsObject::GraphicsObject()
{
}

GraphicsObject::~GraphicsObject()
{
}

void* GraphicsObject::operator new(size_t size)
{
    return ::operator new(size, HeapManager::it().getHeapPtr(ManagedHeap::Type::eGraphics));
}

void* GraphicsObject::operator new[](size_t size)
{
    return ::operator new[](size, HeapManager::it().getHeapPtr(ManagedHeap::Type::eGraphics));
}
