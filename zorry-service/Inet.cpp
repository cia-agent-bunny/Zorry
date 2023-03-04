#include "StdAfx.h"
#include "Inet.h"

#pragma comment(lib, "ws2_32.lib")

Inet::Inet(IN std::wstring& szDestination, IN std::wstring& szService) {
    if (!this->Connect(szDestination, szService)) {
        throw std::exception("failed to connect");
    }
}

Inet::~Inet() {
    this->Close();
}

BOOL Inet::Connect(IN std::wstring& szDestination, IN std::wstring& szService) {
    PADDRINFOW pResult = nullptr;
    ADDRINFOW  Hint = { };

    Hint.ai_family = AF_UNSPEC;
    Hint.ai_protocol = IPPROTO_TCP;
    Hint.ai_socktype = SOCK_STREAM;

    if (GetAddrInfoW(szDestination.c_str(), szService.c_str(), &Hint, &pResult)) {
        ULONG dwLastError = WSAGetLastError();
        
        DEBUG("GetAddrInfoW failed with 0x%08lX (%lu)", dwLastError, dwLastError);
        Log::Print(EVENTLOG_ERROR_TYPE, L"Function 'GetAddrInfoW' failed with 0x%08lX (%lu)", dwLastError, dwLastError);

        return false;
    }

    for (PADDRINFOW pItr = pResult; pItr; pItr = pItr->ai_next) {
        this->mSocket = WSASocketW(pItr->ai_family, pItr->ai_socktype, pItr->ai_protocol, nullptr, 0, 0);

        if (this->mSocket == INVALID_SOCKET) {
            ULONG dwLastError = WSAGetLastError();

            DEBUG("WSASocketW failed with 0x%08lX (%lud)", dwLastError, dwLastError);
            Log::Print(EVENTLOG_ERROR_TYPE, L"Function 'WSASocketW' failed with 0x%08lX (%lu)", dwLastError, dwLastError);    
            
            continue;
        }

        CHAR sBuffer[12] = { };
        
        WSABUF Buffer1 = { };
        Buffer1.buf = sBuffer;
        Buffer1.len = sizeof(sBuffer);

        WSABUF Buffer2 = { };
        Buffer2.buf = sBuffer;
        Buffer2.len = sizeof(sBuffer);

        if (WSAConnect(this->mSocket, pItr->ai_addr, (INT)pItr->ai_addrlen, &Buffer1, &Buffer2, nullptr, nullptr) == SOCKET_ERROR) {
            ULONG dwLastError = WSAGetLastError();

            DEBUG("WSAConnect failed with 0x%08lX (%lud)", dwLastError, dwLastError);
            Log::Print(EVENTLOG_ERROR_TYPE, L"Function 'WSAConnect' failed with 0x%08lX (%lu)", dwLastError, dwLastError);    

            shutdown(this->mSocket, SD_BOTH);
            closesocket(this->mSocket);
            this->mSocket = INVALID_SOCKET;

            continue;
        }

        break;
    }

    FreeAddrInfoW(pResult);

    return true;
}

VOID Inet::Close() {
    if (this->mSocket != INVALID_SOCKET) {
        shutdown(this->mSocket, SD_BOTH);
        closesocket(this->mSocket);
        this->mSocket = INVALID_SOCKET;
    }
}
