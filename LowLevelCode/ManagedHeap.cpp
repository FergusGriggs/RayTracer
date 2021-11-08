#include "ManagedHeap.h"

ManagedHeap::ManagedHeap()
{
}

ManagedHeap::~ManagedHeap()
{
}

void ManagedHeap::updateBytesAllocated(size_t size)
{
	m_bytesAllocated += size;
}

void ManagedHeap::updateBytesDeallocated(size_t size)
{
	m_bytesAllocated -= size;
}
