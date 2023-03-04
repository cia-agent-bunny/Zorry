#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <exception>
#include <mutex>
#include <vector>
#include <sstream>
#include <array>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <intrin.h>

using namespace std::string_literals;

#include "Debug.h"
#include "Log.h"
#include "CAppData.h"
#include "CTextConfigFile.h"

extern CAppData gAppData;
extern CTextConfigFile gConfigFile;
