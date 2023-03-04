#include "StdAfx.h"
#include <locale>
#include <codecvt>
#include "CInetListener.h"

static BOOL InetListenerAcceptProcedure(IN const SOCKET hClient, IN const INET_ADDRESS& Address, IN const InetListenNodeType nNodeType) {
    return true;
}

INT WINAPI wWinMain(IN HINSTANCE hInstance, IN HINSTANCE hPreviousInstance, IN PWSTR szCommandLine, IN INT nCommandShow) {
    DEBUG("started");
    Log::Print(EVENTLOG_INFORMATION_TYPE, L"Started");

    WSADATA WsaData = { };

    if (WSAStartup(MAKEWORD(2, 2), &WsaData)) {
        ULONG dwLastError = WSAGetLastError();

        DEBUG("WSAStartup failed with 0x%08lX (%lu)", dwLastError, dwLastError);
        Log::Print(EVENTLOG_ERROR_TYPE, L"Function 'WSAStartup' failed with 0x%08lX (%lu)", dwLastError, dwLastError);

        return NO_ERROR;
    }

    try {
        if (!gAppData.Init()) {
            return NO_ERROR;
        }

        std::wstring szFilePath;
        
        gAppData.GetConfigFile(szFilePath);

        DEBUG("config file: '%ls'", szFilePath.c_str());

        if (!gConfigFile.Open(szFilePath)) {
            return NO_ERROR;
        }
    } catch (std::exception& e) {
        DEBUG("exception caught: %s", e.what());
        Log::Print(EVENTLOG_ERROR_TYPE, L"Exception caught: %s", e.what());

        return NO_ERROR;
    }

    TextConfigFileString szListenIpPrototype = "";
    TextConfigFileString szListenPortPrototype = "";
    TextConfigFileNumber dwListenIpVersionPrototype = 0;

    if (!gConfigFile.Read("listen-ip"s, szListenIpPrototype)) {
        DEBUG("missing settings field 'listen-ip' value");
        Log::Print(EVENTLOG_ERROR_TYPE, L"Missing settings field 'listen-ip'");

        return NO_ERROR;
    }

    if (!gConfigFile.Read("listen-port"s, szListenPortPrototype)) {
        DEBUG("missing settings field 'listen-port' value");
        Log::Print(EVENTLOG_ERROR_TYPE, L"Missing settings field 'listen-port'");

        return NO_ERROR;
    }

    if (!gConfigFile.Read("listen-ip-version"s, dwListenIpVersionPrototype)) {
        DEBUG("missing settings field 'listen-ip-version' value");
        Log::Print(EVENTLOG_ERROR_TYPE, L"Missing settings field 'listen-ip-version'");

        return NO_ERROR;
    }

    PWSTR szListenIpBuffer = new WCHAR[szListenIpPrototype.size() * sizeof(WCHAR)]{};
    PWSTR szListenPortBuffer = new WCHAR[szListenPortPrototype.size() * sizeof(WCHAR)]{};

    MultiByteToWideChar(CP_UTF7, 0, szListenIpPrototype.c_str(), (INT)szListenIpPrototype.size(), szListenIpBuffer, (INT)szListenIpPrototype.size());
    MultiByteToWideChar(CP_UTF7, 0, szListenPortPrototype.c_str(), (INT)szListenPortPrototype.size(), szListenPortBuffer, (INT)szListenPortPrototype.size());

    std::wstring szListenIp = std::wstring(szListenIpBuffer);
    std::wstring szListenPort = std::wstring(szListenPortBuffer);
    InetListenNodeType nListenNodeType = InetListenNodeType4;

    delete[] szListenIpBuffer;
    delete[] szListenPortBuffer;

    switch (dwListenIpVersionPrototype) {
        case 4: {
            nListenNodeType = InetListenNodeType4;
            break;
        }
        case 6: {
            nListenNodeType = InetListenNodeType6;
            break;
        }
        default: {
            DEBUG("invalid 'listen-ip-version' value");
            Log::Print(EVENTLOG_ERROR_TYPE, L"Invalid settings field 'listen-ip-version', accepted values are '4', and '6'");

            return NO_ERROR;
        }
    }

    while (true) {
        Sleep(1000);

        DEBUG("starting to listen");

        try {
            CInetListener Listener;
            
            if (!Listener.Listen(nListenNodeType, szListenIp, szListenPort, std::function<InetListenAcceptCallbackProc_t>(InetListenerAcceptProcedure))) {
                DEBUG("failed to listen");

                continue;
            }
        } catch (std::exception& e) {
            DEBUG("exception: %s", e.what());
            Log::Print(EVENTLOG_ERROR_TYPE, L"exception: %s", e.what());
        }
    }

    return NO_ERROR;
}
