#include "global_allocation.h"

#include <iostream>

#include "header_footer.h"

#define CHECK_CODE 0xBAADCA75

//#define LOG_MEM_ALLOC

#ifdef LOG_MEM_ALLOC
#define MEM_ALLOC_COUT(cout_msg) std::cout << cout_msg << "\n";
#else
#define MEM_ALLOC_COUT(cout)
#endif


typedef char byte;

void* allocateManaged(size_t size, ManagedHeap* heap)
{
	MEM_ALLOC_COUT("New called with size of " << size)

	size_t fullSize = sizeof(Header) + size + sizeof(Footer);
	byte* memoryAddressHeader = static_cast<byte*>(malloc(fullSize));

	Header* headerPtr = reinterpret_cast<Header*>(memoryAddressHeader);
	if (headerPtr != nullptr)
	{
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
			MEM_ALLOC_COUT("New called with a nullptr heap")
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
	MEM_ALLOC_COUT("Delete called on address " << memoryAddress)
	byte* memoryAddressObject = static_cast<byte*>(memoryAddress);

	Header* headerPtr = reinterpret_cast<Header*>(memoryAddressObject - sizeof(Header));
	if (headerPtr->m_checkCode != CHECK_CODE)
	{
		MEM_ALLOC_COUT("Things went bad with header of " << memoryAddress)
	}

	Footer* footerPtr = reinterpret_cast<Footer*>(memoryAddressObject + headerPtr->m_size);
	if (footerPtr->m_checkCode != CHECK_CODE)
	{
		MEM_ALLOC_COUT("Things went bad with footer of " << memoryAddress)
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
	return allocateManaged(size, &s_defaultHeap);
}

void* operator new[](size_t size)
{
	return allocateManaged(size, &s_defaultHeap);
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