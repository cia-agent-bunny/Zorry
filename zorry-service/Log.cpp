#include "StdAfx.h"

namespace Log {
    static constexpr WCHAR  gszName[] = L"ZORRY";
    static constexpr SIZE_T gstBufferLength = 16 * 1024;
    static std::mutex       gLock = { };
    static WCHAR            gszBuffer[Log::gstBufferLength] = { };

    VOID Print(IN SHORT nType, IN PCWSTR szFormat, IN ...) {
        va_list vaArguments = NULL;
        va_start(vaArguments, szFormat);

        Log::gLock.lock();

        if (_vsnwprintf_s(Log::gszBuffer, Log::gstBufferLength - 1, Log::gstBufferLength, szFormat, vaArguments)) {
            HANDLE hEventSource = RegisterEventSourceW(NULL, Log::gszName);

            if (hEventSource) {
                PCWSTR szStrings[2] = {
                    Log::gszName,
                    Log::gszBuffer
                };

                ReportEventW(hEventSource, nType, 0, 0, NULL, 2, 0, szStrings, NULL);
                DeregisterEventSource(hEventSource);
            }
        }

        Log::gLock.unlock();
        va_end(vaArguments);
    }
}
