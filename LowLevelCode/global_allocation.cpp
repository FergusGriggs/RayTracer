#include "global_allocation.h"

#include <iostream>

#include "header_footer.h"
#include "heap_manager.h"

typedef char byte;

void* allocateManaged(size_t size, ManagedHeap* heap)
{
	size_t fullSize = sizeof(Header) + size + sizeof(Footer);
	byte* memoryAddressHeader = static_cast<byte*>(malloc(fullSize));

	Header* headerPtr = reinterpret_cast<Header*>(memoryAddressHeader);
	if (headerPtr != nullptr)
	{
		MEM_ALLOC_COUT("New called with size of " << size << " on header " << headerPtr << "\n")

		headerPtr->m_checkCode = CHECK_CODE;
		headerPtr->m_size = size;

		headerPtr->m_heapPtr = heap;

		headerPtr->m_next = nullptr;
		headerPtr->m_prev = nullptr;

		if (heap != nullptr)
		{
			heap->updateBytesAllocated(fullSize);
			heap->addHeader(headerPtr);
		}
		else
		{
			MEM_ALLOC_COUT_ERROR("New called with a nullptr heap with on header " << headerPtr << "\n")
		}
	}

	byte* memoryAddressObject = memoryAddressHeader + sizeof(Header);

	Footer* footerPtr = reinterpret_cast<Footer*>(memoryAddressObject + size);
	if (footerPtr != nullptr)
	{
		footerPtr->m_checkCode = CHECK_CODE;
	}

	return static_cast<void*>(memoryAddressObject);
}

void deallocateManaged(void* memoryAddress)
{
	
	byte* memoryAddressObject = static_cast<byte*>(memoryAddress);

	Header* headerPtr = reinterpret_cast<Header*>(memoryAddressObject - sizeof(Header));
	if (headerPtr->m_checkCode != CHECK_CODE)
	{
		MEM_ALLOC_COUT_ERROR("Things went bad with header " << headerPtr << " for object " << memoryAddress)
	}

	Footer* footerPtr = reinterpret_cast<Footer*>(memoryAddressObject + headerPtr->m_size);
	if (footerPtr->m_checkCode != CHECK_CODE)
	{
		MEM_ALLOC_COUT_ERROR("Things went bad with footer " << footerPtr << " for object " << memoryAddress)
	}

	ManagedHeap* heapPtr = headerPtr->m_heapPtr;
	if (heapPtr != nullptr)
	{
		size_t fullSize = sizeof(Header) + headerPtr->m_size + sizeof(Footer);
		heapPtr->updateBytesDeallocated(fullSize);
		heapPtr->removeHeader(headerPtr);
	}

	free(static_cast<void*>(headerPtr));
}

void* operator new(size_t size)
{
	return allocateManaged(size, HeapManager::it().getDefaultHeap());
}

void* operator new[](size_t size)
{
	return allocateManaged(size, HeapManager::it().getDefaultHeap());
}

void* operator new(size_t size, ManagedHeap* heap)
{
	return allocateManaged(size, heap);
}

void* operator new[](size_t size, ManagedHeap* heap)
{
	return allocateManaged(size, heap);
}

void operator delete(void* memoryAddress)
{
	deallocateManaged(memoryAddress);
}

void operator delete[](void* memoryAddress)
{
	return deallocateManaged(memoryAddress);
}