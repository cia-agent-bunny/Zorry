#include "StdAfx.h"

INT WINAPI wWinMain(IN HINSTANCE hInstance, IN HINSTANCE hPreviousInstance, IN PWSTR szCommandLine, IN INT nCommandShow) {
    DEBUG("launched!");
    Log::Print(EVENTLOG_INFORMATION_TYPE, L"Started");

    return NO_ERROR;
}
