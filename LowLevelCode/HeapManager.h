#pragma once

#include "ManagedHeap.h"

class HeapManager
{
public:
	static HeapManager& it()
	{
		static HeapManager staticInstance;
		return staticInstance;
	}

private:
	HeapManager();


};

