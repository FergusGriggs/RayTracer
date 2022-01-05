#pragma once

#include <mutex>

struct Header;

class ManagedHeap
{
public:
	enum class Type
	{
		eDefault,
		eAudio,
		eGraphics,
		ePhysics,
		eInvalid
	};

	ManagedHeap(Type type);
	virtual ~ManagedHeap();

	Type getType() const;

	void updateBytesAllocated(size_t size);
	void updateBytesDeallocated(size_t size);

	std::string getTypeName() const;
	size_t getBytesAllocated() const;

	Header* getLastHeader() const;
	void    addHeader(Header* header);
	void    removeHeader(Header* header);

private:
	Type   m_type;
	size_t m_bytesAllocated;
	
	Header* m_lastHeader;

	std::mutex m_mutex;
};
