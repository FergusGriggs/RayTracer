#pragma once

#include "heap_types/default_heap.h"

#define MAX_HEAPS 10

class HeapManager
{
public:
	~HeapManager();

	static HeapManager& it()
	{
		static HeapManager staticInstance;
		return staticInstance;
	}

	void         initialiseSpecificHeaps();

	void         addHeapPtr(ManagedHeap* heap);
	ManagedHeap* getHeapPtr(ManagedHeap::Type heapType);
	DefaultHeap* getDefaultHeap();

	void printHeapUsages() const;

private:
	HeapManager();

	DefaultHeap m_defaultHeap;

	int          m_numHeaps;
	ManagedHeap* m_allHeaps[MAX_HEAPS];
};

