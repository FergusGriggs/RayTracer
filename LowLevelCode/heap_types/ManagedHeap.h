#pragma once

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

	size_t getBytesAllocated() const;

	Header* getLastHeader() const;
	void    addHeader(Header* header);
	void    removeHeader(Header* header);

private:
	Type   m_type;
	size_t m_bytesAllocated;
	

	Header* m_lastHeader;
};
