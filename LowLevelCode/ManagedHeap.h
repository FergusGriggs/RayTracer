#pragma once

class ManagedHeap
{
public:
	ManagedHeap();
	virtual ~ManagedHeap();

	void updateBytesAllocated(size_t size);
	void updateBytesDeallocated(size_t size);

	size_t m_bytesAllocated;
};

