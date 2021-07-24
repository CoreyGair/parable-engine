

//
//  Implements template funcs for Allocator
//  Included in any .cpp files which use these functions
//

#include "Allocator.h"

namespace Parable
{


template<class T> T* Allocator::allocate_new()
{
    return new (allocate(sizeof(T), alignof(T))) T;
}

template<class T> T* Allocator::allocate_array(size_t length)
{
    PBL_CORE_ASSERT_MSG(length != 0, "Trying to allocate an array of length 0.")

    // we allocate a few extra T's worth of space at the start to store array len
    // the numer of T's needed to store the systems size_t is:
    uint8_t header_size = sizeof(size_t) / sizeof(T);
    if (sizeof(size_t) % sizeof(T) > 0) header_size += 1;

    T* array_start = ((T*) allocate(sizeof(T) * (length + header_size), alignof(T))) + header_size;

    // set the header to the array length
    // cast start to size_t*, move to the bytes before the actual array, set value
    *(((size_t*)array_start) - 1) = length;
    
    // default init all the objects in their array positions
    for (size_t i = 0; i < length; ++i) new (&array_start[i]) T; 
    
    return array_start;
}

template<class T> void Allocator::deallocate_delete(T& object)
{
    object.~T();
    deallocate(&object);
}

template<class T> void Allocator::deallocate_array(T* array)
{
    PBL_CORE_ASSERT_MSG(array != nullptr, "Trying to deallocate a nullptr array.")

    // access the length of the array, stored before the array
    size_t length = *( ((size_t*)array) - 1 ); 

    for (size_t i = 0; i < length; ++i) array[i].~T(); 

    // get the number of extra T's that were alloc'd to store array len
    uint8_t header_size = sizeof(size_t) / sizeof(T);
    if (sizeof(size_t) % sizeof(T) > 0) header_size += 1;

    deallocate(array - header_size); 
}


}