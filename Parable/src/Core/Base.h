#pragma once

// Common includes for internal engine classes

#include <memory>

#include "Core/Log.h"

#define BIT(x) (1 << x)

#define PBL_BIND_EVENT_HANDLER(func) std::bind(std::mem_fn(&func), this, _1)