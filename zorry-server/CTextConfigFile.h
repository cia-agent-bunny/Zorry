#pragma once

using TextConfigFileNumber = LONGLONG;
using TextConfigFileString = std::string;

class CTextConfigFile {
private:
    PVOID mpBlock = nullptr;
    SIZE_T mcbBlock = 0;

public:
    CTextConfigFile();
    CTextConfigFile(IN const std::wstring& szPath);
    ~CTextConfigFile();
    BOOL Open(IN const std::wstring& szPath);
    VOID Close();
    BOOL Read(IN const std::string& szSettingsName, OUT TextConfigFileNumber& Value);
    BOOL Read(IN const std::string& szSettingsName, OUT TextConfigFileString& Value);
};
