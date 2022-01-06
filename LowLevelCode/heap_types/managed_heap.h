#pragma once

#define CHECK_CODE 0xBAADCA75

//#define LOG_MEM_ALLOC_VERBOSE

#ifdef LOG_MEM_ALLOC_VERBOSE
#define MEM_ALLOC_COUT(cout_msg) std::cout << cout_msg << "\n";
#else
#define MEM_ALLOC_COUT(cout)
#endif

#define MEM_ALLOC_COUT_ERROR(cout_msg) std::cout << cout_msg << "\n";

#ifdef __unix
#include <pthread.h>
#else
#include <mutex>
#endif


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

	const char* getTypeName() const;
	size_t getBytesAllocated() const;

	Header* getLastHeader() const;
	void    addHeader(Header* header);
	void    removeHeader(Header* header);

	void    checkHeapIntegrity() const;

private:
	Type   m_type;
	size_t m_bytesAllocated;
	
	Header* m_firstHeader;
	Header* m_lastHeader;

#ifdef __unix
	pthread_mutex_t m_mutex;
#else
	std::mutex m_mutex;
#endif
};
