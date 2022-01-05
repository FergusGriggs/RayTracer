#include "managed_heap.h"

#include "../header_footer.h"

ManagedHeap::ManagedHeap(Type type) :
	m_type(type),
	m_bytesAllocated(0),
	m_lastHeader(nullptr),
	m_mutex()
{
}

ManagedHeap::~ManagedHeap()
{
}

ManagedHeap::Type ManagedHeap::getType() const
{
	return m_type;
}

void ManagedHeap::updateBytesAllocated(size_t size)
{
	m_bytesAllocated += size;
}

void ManagedHeap::updateBytesDeallocated(size_t size)
{
	m_bytesAllocated -= size;
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
	m_mutex.lock();

	header->m_prev = m_lastHeader;

	if (m_lastHeader != nullptr)
	{
		m_lastHeader->m_next = header;
	}
	m_lastHeader = header;

	m_mutex.unlock();
}

void ManagedHeap::removeHeader(Header* header)
{
	m_mutex.lock();

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

	m_mutex.unlock();
}
