#pragma once

#include <cstdint>


/**
 * @file Pointer.h
 *
 * Defines utility functions for pointer manipulation.
 */


namespace Parable::Util
{


/**
 * Aligns a pointer unsafely, without bounds/size checking.
 *
 * Acts as a replacement for std::align(), without the need to pass buffer size and object size.
 *
 * @return void* the aligned pointer
 */
inline uintptr_t manual_align(size_t alignment, uintptr_t ptr)
{
	if (ptr % alignment != 0)
	{
		ptr += alignment - ptr % alignment;
	}

	return ptr;
}


inline void* manual_align(size_t alignment, void* ptr)
{
	return (void*)manual_align(alignment, (uintptr_t)ptr);
}



}