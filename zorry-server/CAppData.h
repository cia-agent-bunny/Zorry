#pragma once

class CAppData {
private:
    std::wstring mszRootPath = L""s;
    std::wstring mszLogPath = L""s;

public:
    CAppData();
    ~CAppData();
    BOOL Init();
    VOID GetConfigFile(OUT std::wstring& szPath);
};
