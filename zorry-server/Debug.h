#pragma once

#ifdef _DEBUG

namespace Debug {
    VOID Print(IN PCSTR szFunction, IN PCSTR szFormat, IN ...);
}

#endif

#ifdef _DEBUG
#define DEBUG(FORMAT, ...) Debug::Print(__FUNCTION__, FORMAT "\n", __VA_ARGS__);
#else
#define DEBUG(FORMAT, ...) __noop(__FUNCTION__, FORMAT "\n", __VA_ARGS__);
#endif
