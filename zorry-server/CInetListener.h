#pragma once

#include <functional>

enum InetListenNodeType {
    InetListenNodeType4,
    InetListenNodeType6,
};

union INET_ADDRESS {
    sockaddr_in  IN4;
    sockaddr_in6 IN6;
};

using InetListenAcceptCallbackProc_t = BOOL(IN const SOCKET hClient, IN const INET_ADDRESS& Address, IN const InetListenNodeType nNodeType);

class CInetListener {
private:
    volatile SOCKET mListeningSocket = INVALID_SOCKET;

public:
    CInetListener();
    CInetListener(IN InetListenNodeType nNodeType, IN const std::wstring& szListeningIp, IN const std::wstring& szService, IN const std::function<InetListenAcceptCallbackProc_t>& fnCallback);
    ~CInetListener();
    BOOL Listen(IN InetListenNodeType nNodeType, IN const std::wstring& szListeningIp, IN const std::wstring& szService, IN const std::function<InetListenAcceptCallbackProc_t>& fnCallback);
    VOID Close();
};
