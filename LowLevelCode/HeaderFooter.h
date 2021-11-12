#pragma once

#include "heap_types/ManagedHeap.h"

struct Header
{
	unsigned int m_checkCode;
	size_t       m_size;

	ManagedHeap* m_heapPtr;

	Header* m_prev;
	Header* m_next;
};

struct Footer
{
	unsigned int m_checkCode;
};