#include "keyhook.hpp"

namespace {
	HWND window = NULL;
	HHOOK hook_proc = NULL;
}

namespace app {

	
	LRESULT CALLBACK keyboard_proc(int _code, WPARAM _wparam, LPARAM _lparam)
	{
		if (_code != HC_ACTION)
			return ::CallNextHookEx(hook_proc, _code, _wparam, _lparam);

		KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)_lparam;

		if (_wparam == WM_KEYDOWN)
		{
			switch (kbd->vkCode)
			{
			case VK_F21:
			case VK_F22:
			case VK_F23:
			case VK_F24:
				::PostMessageW(window, CWM_GLOBAL_KEYDOWN, kbd->vkCode, 0);
				break;
			}
		}
		else if (_wparam == WM_KEYUP)
		{
			switch (kbd->vkCode)
			{
			case VK_F21:
			case VK_F22:
			case VK_F23:
			case VK_F24:
				::PostMessageW(window, CWM_GLOBAL_KEYUP, kbd->vkCode, 0);
				break;
			}
		}

		return ::CallNextHookEx(hook_proc, _code, _wparam, _lparam);
	}

	keyhook::keyhook()
	{
	}

	keyhook::~keyhook()
	{
		if (hook_proc != NULL)
		{
			unhook();
		}
	}

	bool keyhook::hook(HINSTANCE _instance, HWND _window)
	{
		window = _window;

		if (hook_proc != NULL) return true;

		hook_proc = ::SetWindowsHookExW(WH_KEYBOARD_LL, (HOOKPROC)keyboard_proc, _instance, 0);
		return hook_proc != NULL;
	}

	bool keyhook::unhook()
	{
		if (hook_proc == NULL) return true;

		auto rc = ::UnhookWindowsHookEx(hook_proc);
		if (rc == TRUE)
		{
			hook_proc = NULL;
		}
		return rc == TRUE;
	}
}
