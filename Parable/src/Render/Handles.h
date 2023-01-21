#pragma once

#include <pblpch.h>


namespace Parable
{


#define SIMPLE_HANDLE(class,member) struct class {                                                              \
                                        uint64_t member;                                                        \
                                        bool operator==(const class& other) { return member == other.member; }  \
                                        bool operator!=(const class& other) { return member != other.member; }  \
                                    }                                                                           \

SIMPLE_HANDLE(MeshHandle,mesh);

SIMPLE_HANDLE(MaterialHandle,material);


}