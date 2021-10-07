#pragma once

#define WINVER       0x0A00 // windows10
#define _WIN32_WINNT 0x0A00 // windows10

#include <WinSDKVer.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")

namespace app {
	constexpr UINT CWM_GLOBAL_KEYDOWN      = WM_APP + 1;
	constexpr UINT CWM_GLOBAL_KEYUP        = WM_APP + 2;
	constexpr UINT CWM_TASKTRAY            = WM_APP + 3;
}
