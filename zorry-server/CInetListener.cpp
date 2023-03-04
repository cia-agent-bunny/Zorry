#include "StdAfx.h"
#include "CInetListener.h"

#pragma comment(lib, "ws2_32.lib")

CInetListener::CInetListener() { }

CInetListener::CInetListener(IN InetListenNodeType nNodeType, IN const std::wstring& szListeningIp, IN const std::wstring& szService, IN const std::function<InetListenAcceptCallbackProc_t>& fnCallback) {
    if (this->Listen(nNodeType, szListeningIp, szService, fnCallback)) {
        throw std::exception("failed to connect");
    }
}

CInetListener::~CInetListener() {
    this->Close();
}

BOOL CInetListener::Listen(IN InetListenNodeType nNodeType, IN const std::wstring& szListeningIp, IN const std::wstring& szService, IN const std::function<InetListenAcceptCallbackProc_t>& fnCallback) {
    switch (nNodeType) {
        case InetListenNodeType4: {
            this->mListeningSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);

            if (this->mListeningSocket == INVALID_SOCKET) { 
                ULONG dwLastError = WSAGetLastError();

                DEBUG("WSASocketW failed with 0x%08lX (%lu)", dwLastError, dwLastError);
                Log::Print(EVENTLOG_ERROR_TYPE, L"Function 'WSASocketW' failed with 0x%08lX (%lu)", dwLastError, dwLastError);

                shutdown(this->mListeningSocket, SD_BOTH);
                closesocket(this->mListeningSocket);
                this->mListeningSocket = INVALID_SOCKET;

                return false;
            }

            SOCKADDR_IN Address = { };
            INT         cbAddress = sizeof(Address);

            if (WSAStringToAddressW((PWSTR)szListeningIp.c_str(), AF_INET, NULL, (PSOCKADDR)&Address, &cbAddress) == SOCKET_ERROR) {
                ULONG dwLastError = WSAGetLastError();

                DEBUG("WSAStringToAddressW failed with 0x%08lX (%lu)", dwLastError, dwLastError);
                Log::Print(EVENTLOG_ERROR_TYPE, L"Function 'WSAStringToAddressW' failed with 0x%08lX (%lu)", dwLastError, dwLastError);

                shutdown(this->mListeningSocket, SD_BOTH);
                closesocket(this->mListeningSocket);
                this->mListeningSocket = INVALID_SOCKET;

                throw std::exception("invalid argumetn passed (szListeningIp)");

                return false;
            }

            Address.sin_port = htons((u_short)std::stoi(szService));

            if (bind(this->mListeningSocket, (PSOCKADDR)&Address, cbAddress) == SOCKET_ERROR) {
                ULONG dwLastError = WSAGetLastError();

                DEBUG("bind failed with 0x%08lX (%lu)", dwLastError, dwLastError);
                Log::Print(EVENTLOG_ERROR_TYPE, L"Function 'bind' failed with 0x%08lX (%lu)", dwLastError, dwLastError);

                shutdown(this->mListeningSocket, SD_BOTH);
                closesocket(this->mListeningSocket);
                this->mListeningSocket = INVALID_SOCKET;

                return false;
            }

            if (listen(this->mListeningSocket, SOMAXCONN) == SOCKET_ERROR) {
                ULONG dwLastError = WSAGetLastError();

                DEBUG("listen failed with 0x%08lX (%lu)", dwLastError, dwLastError);
                Log::Print(EVENTLOG_ERROR_TYPE, L"Function 'listen' failed with 0x%08lX (%lu)", dwLastError, dwLastError);

                shutdown(this->mListeningSocket, SD_BOTH);
                closesocket(this->mListeningSocket);
                this->mListeningSocket = INVALID_SOCKET;

                return false;            
            }

            while (TRUE) {
                SOCKADDR_IN ClientAddress = { };
                INT         cbClientAddress = sizeof(ClientAddress);

                SOCKET hClientSocket = WSAAccept(this->mListeningSocket, (PSOCKADDR)&ClientAddress, &cbClientAddress, nullptr, 0);

                if (hClientSocket == INVALID_SOCKET) {
                    if (this->mListeningSocket == INVALID_SOCKET) {
                        break;
                    }

                    DEBUG("failed to accept");

                    continue;
                }

                INET_ADDRESS FlexibleAddress = { };
                memcpy(&FlexibleAddress.IN4, &ClientAddress, cbClientAddress);

                if (!fnCallback(hClientSocket, FlexibleAddress, InetListenNodeType4)) {
                    shutdown(hClientSocket, SD_BOTH);
                    closesocket(hClientSocket);
                    hClientSocket = INVALID_SOCKET;

                    WCHAR szAddressFriendly[64] = L"N/A";
                    ULONG nbAddressFriendly = ARRAYSIZE(szAddressFriendly);

                    WSAAddressToStringW((PSOCKADDR)&ClientAddress, cbClientAddress, nullptr, szAddressFriendly, &nbAddressFriendly);
                    DEBUG("connection refused with , %ls", szAddressFriendly);
                    Log::Print(EVENTLOG_WARNING_TYPE, L"Connection refused with '%ls'", szAddressFriendly);

                    continue;
                }
            }

            break;
        }
        default: {
            throw std::exception("invalid argument passed (nNodeType)");
            return false;
        }
    }

    return true;
}

VOID CInetListener::Close() {
    if (this->mListeningSocket != INVALID_SOCKET) {
        shutdown(this->mListeningSocket, SD_BOTH);
        closesocket(this->mListeningSocket);
        this->mListeningSocket = INVALID_SOCKET;
    }
}