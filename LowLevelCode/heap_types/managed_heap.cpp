#include "managed_heap.h"

#include "../header_footer.h"

#include <iostream>

ManagedHeap::ManagedHeap(Type type) :
	m_type(type),
	m_bytesAllocated(0),
	m_firstHeader(nullptr),
	m_lastHeader(nullptr),
	m_mutex()
{
	std::cout << "Constructed " << getTypeName() << " heap\n";
}

ManagedHeap::~ManagedHeap()
{
	std::cout << "Deconstructed " << getTypeName() << " heap. ";

	std::cout << "It had " << m_bytesAllocated << " bytes allocated. ";

	if (m_bytesAllocated != 0)
	{
		std::cout << "(Try using delete next time kid) ";
	}

	checkHeapIntegrity();
}

ManagedHeap::Type ManagedHeap::getType() const
{
	return m_type;
}

void ManagedHeap::updateBytesAllocated(size_t size)
{
#ifdef __unix
	pthread_mutex_lock(&m_mutex);
	m_bytesAllocated += size;
	pthread_mutex_unlock(&m_mutex);
#else
	m_mutex.lock();
	m_bytesAllocated += size;
	m_mutex.unlock();
#endif
}

void ManagedHeap::updateBytesDeallocated(size_t size)
{
#ifdef __unix
	pthread_mutex_lock(&m_mutex);
	m_bytesAllocated -= size;
	pthread_mutex_unlock(&m_mutex);
#else
	m_mutex.lock();
	m_bytesAllocated -= size;
	m_mutex.unlock();
#endif
}

const char* ManagedHeap::getTypeName() const
{
	switch (m_type)
	{
	case ManagedHeap::Type::eDefault:
		return "eDefault";
	case ManagedHeap::Type::eGraphics:
		return "eGraphics";
	case ManagedHeap::Type::eAudio:
		return "eAudio";
	case ManagedHeap::Type::ePhysics:
		return "ePhysics";
	}
	return "eInvalid";
}

size_t ManagedHeap::getBytesAllocated() const
{
	return m_bytesAllocated;
}

Header* ManagedHeap::getLastHeader() const
{
	return m_lastHeader;
}

void ManagedHeap::addHeader(Header* header)
{
#ifdef __unix
	pthread_mutex_lock(&m_mutex);
#else
	m_mutex.lock();
#endif

	if (m_firstHeader == nullptr)
	{
		m_firstHeader = header;
	}

	header->m_prev = m_lastHeader;

	if (m_lastHeader != nullptr)
	{
		m_lastHeader->m_next = header;
	}
	m_lastHeader = header;

#ifdef __unix
	pthread_mutex_unlock(&m_mutex);
#else
	m_mutex.unlock();
#endif
}

void ManagedHeap::removeHeader(Header* header)
{
#ifdef __unix
	pthread_mutex_lock(&m_mutex);
#else
	m_mutex.lock();
#endif

	// Update first header
	if (header == m_firstHeader)
	{
		m_firstHeader = header->m_next;
	}

	// Update previous header
	if (header->m_prev != nullptr)
	{
		header->m_prev->m_next = header->m_next;
	}

	// Update next header
	if (header->m_next != nullptr)
	{
		header->m_next->m_prev = header->m_prev;
	}

	// Update last header
	if (header == m_lastHeader)
	{
		m_lastHeader = header->m_prev;
	}

#ifdef __unix
	pthread_mutex_unlock(&m_mutex);
#else
	m_mutex.unlock();
#endif
}

void ManagedHeap::checkHeapIntegrity() const
{
	// Walking the heap
	int bytesCounted = 0;
	int headerFooterSize = sizeof(Header) + sizeof(Footer);
	bool valid = true;
	Header* header = m_firstHeader;
	while (header != nullptr)
	{
		bytesCounted += header->m_size + headerFooterSize;
		
		MEM_ALLOC_COUT("Header " << header << " had size " << header->m_size << "\n");

		if (header->m_checkCode != CHECK_CODE)
		{
			MEM_ALLOC_COUT_ERROR("Failed to walk heap, header: " << header << " was invalid\n");
			valid = false;
		}

		header = header->m_next;
	}

	if (bytesCounted != m_bytesAllocated)
	{
		MEM_ALLOC_COUT_ERROR("Failed to walk heap, bytes counted was " << bytesCounted << " but member allocated bytes is " << m_bytesAllocated << "\n");
		valid = false;
	}

	std::cout << "Heap walked with no issues\n";
}
