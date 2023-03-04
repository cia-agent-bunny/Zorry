#include "StdAfx.h"

#ifdef _DEBUG

namespace Debug {
    static constexpr SIZE_T gstBufferLength = 16 * 1024;
    static volatile LONG    gdwBufferLock = FALSE;
    static CHAR             gszBuffer[Debug::gstBufferLength] = { };

    VOID Print(IN PCSTR szFunction, IN PCSTR szFormat, IN ...) {
        va_list vaArguments = NULL;
        va_start(vaArguments, szFormat);

        while (InterlockedCompareExchange(&Debug::gdwBufferLock, TRUE, FALSE)) {
            _mm_pause();
        }

        INT nMessageOffset = _snprintf_s(Debug::gszBuffer, Debug::gstBufferLength - 1, Debug::gstBufferLength, "ZORRY!%s : ", szFunction);

        if (nMessageOffset) {
            if (_vsnprintf_s(Debug::gszBuffer + nMessageOffset, Debug::gstBufferLength - 1 - nMessageOffset, Debug::gstBufferLength - nMessageOffset, szFormat, vaArguments)) {
                OutputDebugStringA(Debug::gszBuffer);
            }
        }

        InterlockedExchange(&Debug::gdwBufferLock, FALSE);
        va_end(vaArguments);
    }
}

#endif
