#pragma once

#include "heap_types/managed_heap.h"

struct Header
{
	unsigned int m_checkCode;
	size_t       m_size;

	ManagedHeap* m_heapPtr;

	Header* m_prev;
	Header* m_next;

	bool m_isPoolAllocated;
};

struct Footer
{
	unsigned int m_checkCode;
};