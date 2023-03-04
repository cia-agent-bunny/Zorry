#include "StdAfx.h"
#include "CTextConfigFile.h"

CTextConfigFile::CTextConfigFile() { }

CTextConfigFile::CTextConfigFile(IN const std::wstring& szPath) {
    if (!this->Open(szPath)) {
        throw std::exception("failed to open text configuration file");
    }
}

CTextConfigFile::~CTextConfigFile() {
    this->Close();
}

BOOL CTextConfigFile::Open(IN const std::wstring& szPath) {
    HANDLE hFile = CreateFileW(szPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        ULONG dwLastError = GetLastError();

        DEBUG("failed to access file '%ls', error 0x%08lX (%lu)", szPath.c_str(), dwLastError, dwLastError);
        Log::Print(EVENTLOG_ERROR_TYPE, L"Failed to access file '%ls', error 0x%08lX (%lu)", szPath.c_str(), dwLastError, dwLastError);

        return false;
    }

    ULONG dwFileSizeHigh = 0;
    ULONG dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

    if (!dwFileSize || dwFileSizeHigh) {
        DEBUG("file '%ls' is above 4 GB of it's empty", szPath.c_str());
        Log::Print(EVENTLOG_ERROR_TYPE, L"The file '%ls' is above 4 GiB of it is empty", szPath.c_str());

        CloseHandle(hFile);

        return false;
    }

    this->mcbBlock = dwFileSize;
    this->mpBlock = VirtualAlloc(nullptr, this->mcbBlock, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!this->mpBlock) {
        ULONG dwLastError = GetLastError();

        DEBUG("failed to allocate memory for '%ls', error 0x%08lX (%lu)", szPath.c_str(), dwLastError, dwLastError);
        Log::Print(EVENTLOG_ERROR_TYPE, L"Failed to allocate memory for '%ls', error 0x%08lX (%lu)", szPath.c_str(), dwLastError, dwLastError);

        CloseHandle(hFile);

        return false;
    }

    ULONG dwBytesRead = 0;

    if (!ReadFile(hFile, this->mpBlock, (ULONG)this->mcbBlock, &dwBytesRead, NULL)) {
        ULONG dwLastError = GetLastError();

        DEBUG("failed to read from '%ls', error 0x%08lX (%lu)", szPath.c_str(), dwLastError, dwLastError);
        Log::Print(EVENTLOG_ERROR_TYPE, L"Failed to read from '%ls', error 0x%08lX (%lu)", szPath.c_str(), dwLastError, dwLastError);

        CloseHandle(hFile);
        VirtualFree(this->mpBlock, 0, MEM_RELEASE);

        return false;
    }

    CloseHandle(hFile);

    ULONG dwOldProect = 0;

    if (!VirtualProtect(this->mpBlock, this->mcbBlock, PAGE_READONLY, &dwOldProect)) {
        ULONG dwLastError = GetLastError();
        
        DEBUG("failed to make the content of the file '%ls' read-only, error 0x%08lX (%lu)", szPath.c_str(), dwLastError, dwLastError);
        Log::Print(EVENTLOG_WARNING_TYPE, L"Failed to make the content of the file '%ls' read-only, error 0x%08lX (%lu)", szPath.c_str(), dwLastError, dwLastError);
    }

    return true;
}

VOID CTextConfigFile::Close() {
    if (this->mpBlock) {
        VirtualFree(this->mpBlock, 0, MEM_RELEASE);
        this->mpBlock = nullptr;
        this->mcbBlock = 0;
    }
}

BOOL CTextConfigFile::Read(IN const std::string& szSettingsName, OUT TextConfigFileNumber& Value) {
    std::string szFileContent((PSTR)this->mpBlock, this->mcbBlock);
    std::stringstream ss(szFileContent);
    std::string szLine;
    
    while (std::getline(ss, szLine)) {
        SIZE_T stSplitOffset = szLine.find('=');

        if (!szLine.compare(0, stSplitOffset, szSettingsName.c_str())) {
            std::string szString = szLine.substr(stSplitOffset + 1);

            INT nTrimLeftUntilIndex = 0;
            INT nTrimRightFromIndex = (INT)szString.length();

            for (auto itr = szString.begin(); itr < szString.end(); itr++) {
                CHAR cChar = *itr;

                if (cChar <= ' ') {
                    continue;
                }

                nTrimLeftUntilIndex++;

                break;
            }

            for (auto itr = szString.rbegin(); itr < szString.rend(); itr++) {
                CHAR cChar = *itr;

                if (cChar < ' ') {
                    continue;
                }

                nTrimRightFromIndex--;

                break;
            }

            Value = std::strtoll(szString.c_str(), nullptr, 10);

            return true;
        }
    }

    return false;
}

BOOL CTextConfigFile::Read(IN const std::string& szSettingsName, OUT TextConfigFileString& Value) {
    std::string szFileContent((PSTR)this->mpBlock, this->mcbBlock);
    std::stringstream ss(szFileContent);
    std::string szLine;
    
    while (std::getline(ss, szLine)) {
        SIZE_T stSplitOffset = szLine.find('=');

        if (!szLine.compare(0, stSplitOffset, szSettingsName.c_str())) {
            std::string szString = szLine.substr(stSplitOffset + 1);

            INT nTrimLeftUntilIndex = 0;
            INT nTrimRightFromIndex = (INT)szString.length();

            for (auto itr = szString.begin(); itr < szString.end(); itr++) {
                CHAR cChar = *itr;

                if (cChar <= ' ') {
                    nTrimLeftUntilIndex++;

                    continue;
                }


                break;
            }

            for (auto itr = szString.rbegin(); itr < szString.rend(); itr++) {
                CHAR cChar = *itr;

                if (cChar < ' ') {
                    nTrimRightFromIndex--;

                    continue;
                }

                break;
            }

            szString.erase(nTrimRightFromIndex);
            Value = std::string(szString.c_str() + nTrimLeftUntilIndex);

            return true;
        }
    }

    return false;
}
