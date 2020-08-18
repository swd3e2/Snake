#pragma once

#include <assert.h>

class LinearAllocator {
private:
	void* mem = nullptr;
	void* cursor = nullptr;
	size_t size = 0;
public:
	LinearAllocator(size_t size) :
		size(size)
	{
		mem = _aligned_malloc(size, 4);
		cursor = mem;
	}

	void* allocate(int size)
	{
		void* memory = cursor;
		cursor = (void*)((uintptr_t)cursor + size);

		return memory;
	}

	void deallocate(void* memory)
	{
		assert("Could not deallocate memory allocated with linear allocator");
	}

	void clear()
	{
		cursor = mem;
	}
};