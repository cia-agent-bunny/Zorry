#pragma once

class Inet {
private:
    SOCKET mSocket = INVALID_SOCKET;

public:
    Inet() = delete;
    Inet(IN std::wstring& szDestination, IN std::wstring& szService);
    ~Inet();
    BOOL Connect(IN std::wstring& szDestination, IN std::wstring& szService);
    VOID Close();
};
