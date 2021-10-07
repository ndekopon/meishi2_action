#include "common.hpp"

#include <Tlhelp32.h>

#include "terminate.hpp"

namespace app {
	
	void terminate_exe(const std::wstring& _exefile)
	{
        PROCESSENTRY32W entry;
        entry.dwSize = sizeof(entry);

        auto snaphandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snaphandle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        auto result = ::Process32FirstW(snaphandle, &entry);
        while (result == TRUE)
        {
            std::wstring exefile = entry.szExeFile;

            if (exefile == _exefile)
            {
                auto processhandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                
                if (processhandle != NULL)
                {
                    // 終了
                    if (::TerminateProcess(processhandle, 0) == TRUE)
                    {
                        DWORD exitcode = 0;
                        ::GetExitCodeProcess(processhandle, &exitcode);
                    }
                    ::CloseHandle(processhandle);
                }
            }

            result = ::Process32NextW(snaphandle, &entry);
        }
        ::CloseHandle(snaphandle);

        return;
	}

}
