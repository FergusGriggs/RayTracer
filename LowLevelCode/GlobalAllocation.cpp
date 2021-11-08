#include "GlobalAllocation.h"

#include <iostream>

#include "HeaderFooter.h"

#define CHECK_CODE 0xBAADCA75

typedef char byte;

void* operator new(size_t size)
{
	std::cout << "New called with size of " << size << "\n";
	size_t fullSize = sizeof(Header) + size + sizeof(Footer);
	byte* memoryAddressHeader = static_cast<byte*>(malloc(fullSize));

	Header* headerPtr = reinterpret_cast<Header*>(memoryAddressHeader);
	if (headerPtr != nullptr)
	{
		headerPtr->m_checkCode = CHECK_CODE;
		headerPtr->m_size = size;
	}

	byte* memoryAddressObject = memoryAddressHeader + sizeof(Header);

	Footer* footerPtr = reinterpret_cast<Footer*>(memoryAddressObject + size);
	if (footerPtr != nullptr)
	{
		footerPtr->m_checkCode = CHECK_CODE;
	}

	return static_cast<void*>(memoryAddressObject);
}

void operator delete(void* memoryAddress)
{
	std::cout << "Delete called on address " << memoryAddress << "\n";

	byte* memoryAddressObject = static_cast<byte*>(memoryAddress);

	Header* headerPtr = reinterpret_cast<Header*>(memoryAddressObject - sizeof(Header));
	if (headerPtr->m_checkCode != CHECK_CODE)
	{
		std::cout << "Things went bad with header of " << memoryAddress << "\n";
	}

	Footer* footerPtr = reinterpret_cast<Footer*>(memoryAddressObject + headerPtr->m_size);
	if (footerPtr->m_checkCode != CHECK_CODE)
	{
		std::cout << "Things went bad with footer of " << memoryAddress << "\n";
	}
	
	free(static_cast<void*>(headerPtr));
}