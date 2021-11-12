#pragma once

#include <unordered_map>

#include "heap_types/DefaultHeap.h"

static DefaultHeap s_defaultHeap;

class HeapManager
{
public:
	~HeapManager();

	static HeapManager& it()
	{
		static HeapManager staticInstance;
		return staticInstance;
	}

	void         addHeapPtr(ManagedHeap* heap);
	ManagedHeap* getHeapPtr(ManagedHeap::Type heapType);

private:
	HeapManager();

	std::unordered_map<ManagedHeap::Type, ManagedHeap*> m_heaps;
};

