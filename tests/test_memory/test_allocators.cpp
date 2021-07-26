// #include <catch2/catch_test_macros.hpp>



// //
// // TODO: see if u can fix the segfaults these tests cause
// //
// //    Think its catch2 not liking the malloc, but not sure
// //





// #include "Memory/LinearAllocator.h"
// #include "Memory/PoolAllocator.h"

// #include "Memory/Allocator.tpp"

// //TMP
// #include <stdio.h>

// void* mem = malloc(128);
// Parable::LinearAllocator alloc (128, mem);

// TEST_CASE("LinearAllocator", "[memory]")
// {
//     alloc.clear();

//     // NOTE: we dont test deallocation here as LinearAllocator doesnt dealloc, only clear

//     SECTION("Allocate")
//     {
//         int* x = alloc.allocate_new<int>();
//         REQUIRE(x != nullptr);
//     }
//     SECTION("Allocate array")
//     {
//         int* arr = alloc.allocate_array<int>(4);
//         REQUIRE((&arr[0] != nullptr && &arr[1] != nullptr && &arr[2] != nullptr && &arr[3] != nullptr));
//     }
// }

// TEST_CASE("PoolAllocator", "[memory]")
// {
//     alloc.clear();

//     SECTION("Allocate")
//     {
//         int* x = alloc.allocate_new<int>();
//         REQUIRE(x != nullptr);
        
//         SECTION("Deallocate")
//         {
//             alloc.deallocate_delete(*x);
//             REQUIRE((alloc.get_used() == 0 && alloc.get_allocations() == 0));
//         }
//     }
//     SECTION("Allocate array")
//     {
//         int* arr = alloc.allocate_array<int>(4);
//         REQUIRE((&arr[0] != nullptr && &arr[1] != nullptr && &arr[2] != nullptr && &arr[3] != nullptr));

//         SECTION("Deallocate array")
//         {
//             alloc.deallocate_array(arr);
//             REQUIRE((alloc.get_used() == 0 && alloc.get_allocations() == 0));
//         }
//     }
// }