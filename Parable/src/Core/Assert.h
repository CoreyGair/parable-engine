#pragma once

#include "Core/Base.h"
#include "Core/Log.h"

// simple for now to enable assertions in debug builds
#ifdef PBL_DEBUG
    #define PBL_ENABLE_ASSERTS
#endif

#ifdef PBL_ENABLE_ASSERTS
    #define PBL_ASSERT_MSG(pred,...) { if (!(pred)) { PBL_CORE_ERROR(__VA_ARGS__); PBL_DEBUGBREAK(); } }
    #define PBL_ASSERT(pred) PBL_ASSERT_MSG(pred,"Assertion failed! Line {} in file '{}'", __LINE__, __FILE__)
#else
    #define PBL_ASSERT_MSG(pred,msg)
    #define PBL_ASSERT(pred)
#endif