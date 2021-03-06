#pragma once

#include "heap_manager.h"

static void* allocateManaged(size_t size, ManagedHeap* heap);
static void deallocateManaged(void* memoryAddress);

void* operator new(size_t size);
void* operator new[](size_t size);

void* operator new(size_t size, ManagedHeap* heap);
void* operator new[](size_t size, ManagedHeap* heap);

void operator delete(void* memoryAddress) noexcept;
void operator delete[](void* memoryAddress) noexcept;
