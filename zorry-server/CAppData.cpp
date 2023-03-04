#include "StdAfx.h"
#include <filesystem>
#include "CAppData.h"
    
static const std::wstring gszDataFolderName = L"Zorry-Data"s;
static const std::wstring gszTextConfigFilename = L"config.txt"s;

CAppData::CAppData() { }

BOOL CAppData::Init() {
    std::array<WCHAR, MAX_PATH> sAppDataPathBuffer = { };

    if (!GetEnvironmentVariableW(L"APPDATA", sAppDataPathBuffer.data(), (ULONG)sAppDataPathBuffer.size())) {
        ULONG dwLastError = GetLastError();

        DEBUG("missing environment variable 'APPDATA' in the environment, function 'GetEnvironmentVariableW' error: 0x%08lX (%lu)", dwLastError, dwLastError);
        Log::Print(EVENTLOG_ERROR_TYPE, L"Missing environment variable 'APPDATA' in the environment, function 'GetEnvironmentVariableW' error: 0x%08lX (%lu)", dwLastError, dwLastError);
        
        throw std::exception("missing APPDATA folder in environment");
        
        return false;
    }

    const std::wstring szAppDataFolder = std::wstring(sAppDataPathBuffer.data()) + L"\\"s;
    this->mszRootPath = szAppDataFolder + gszDataFolderName;
    this->mszLogPath = this->mszRootPath + L"\\log"s;

    if (!CreateDirectoryW(this->mszRootPath.c_str(), NULL)) {
        ULONG dwLastError = GetLastError();

        if (dwLastError == ERROR_PATH_NOT_FOUND) {
            DEBUG("missing path, function 'CreateDirectoryW' returned 0x%08lX (%lu)", dwLastError, dwLastError);
            Log::Print(EVENTLOG_ERROR_TYPE, L"Missing Path, function 'CreateDirectoryW' returned 0x%08lX (%lu)", dwLastError, dwLastError);

            throw std::exception("missing %appdata%\\zorry path");

            return false;
        }
    }

    if (!CreateDirectoryW(this->mszLogPath.c_str(), NULL)) {
        ULONG dwLastError = GetLastError();

        if (dwLastError == ERROR_PATH_NOT_FOUND) {
            DEBUG("missing path, function 'CreateDirectoryW' returned 0x%08lX (%lu)", dwLastError, dwLastError);
            Log::Print(EVENTLOG_ERROR_TYPE, L"Missing Path, function 'CreateDirectoryW' returned 0x%08lX (%lu)", dwLastError, dwLastError);

            throw std::exception("missing %appdata%\\zorry path");

            return false;
        }
    }

    return true;
}

CAppData::~CAppData() {
    this->mszRootPath.clear();
    this->mszLogPath.clear();
}

VOID CAppData::GetConfigFile(OUT std::wstring& szPath) {
    szPath = this->mszRootPath + L"\\"s + gszTextConfigFilename;
}
