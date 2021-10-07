#include "common.hpp"

#include <vector>

#include <imm.h>
#include <mmsystem.h>

#include "mic_volume.hpp"
#include "main_window.hpp"
#include "terminate.hpp"

#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "Winmm.lib")

namespace app {

	constexpr UINT MID_EXIT = 0;

	const wchar_t* main_window::window_class_ = L"meishi2_actionmainwindow";
	const wchar_t* main_window::window_title_ = L"meishi2_action";
	const wchar_t* main_window::window_mutex_ = L"meishi2_actionmutex";


	main_window::main_window(HINSTANCE _instance)
		: instance_(_instance)
		, window_(nullptr)
		, width_(0)
		, height_(0)
		, volume_()
		, khook_()
		, nid_({ 0 })
		, taskbar_created_(0)
		, wav_mute_(L"")
		, wav_unmute_(L"")
	{
		wav_mute_ = get_full_path(L"mute.wav");
		wav_unmute_ = get_full_path(L"unmute.wav");
	}

	main_window::~main_window()
	{
	}

	bool main_window::init()
	{

		HANDLE mutex = ::CreateMutexW(NULL, TRUE, window_mutex_);
		if (::GetLastError() == ERROR_ALREADY_EXISTS) {
			return false;
		}

		disable_ime();

		set_dpi_awareness();

		// create window
		register_window_class();
		if (!create_window())
			return false;

		return true;
	}


	int main_window::loop()
	{
		MSG message;

		while (::GetMessageW(&message, nullptr, 0, 0))
		{
			::TranslateMessage(&message);
			::DispatchMessageW(&message);
		}
		return (int)message.wParam;
	}

	void main_window::disable_ime()
	{
		::ImmDisableIME(-1);
	}

	void main_window::set_dpi_awareness()
	{
		auto desired_context = DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED;
		if (::IsValidDpiAwarenessContext(desired_context))
		{
			auto hr = ::SetProcessDpiAwarenessContext(desired_context);
			if (hr)
				return;
		}
	}

	ATOM main_window::register_window_class()
	{
		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEXW);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = window_proc_common;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = instance_;
		wcex.hIcon = ::LoadIconW(nullptr, IDI_APPLICATION);
		wcex.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
		wcex.hbrBackground = nullptr;
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = window_class_;
		wcex.hIconSm = ::LoadIconW(nullptr, IDI_APPLICATION);

		return ::RegisterClassExW(&wcex);
	}

	bool main_window::create_window()
	{
		window_ = ::CreateWindowExW(0, window_class_, window_title_, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, instance_, this);

		if (window_ == nullptr)
		{
			return false;
		}

		return true;
	}

	UINT main_window::tasktray_add()
	{
		nid_.cbSize = sizeof(NOTIFYICONDATAW);
		nid_.uFlags = (NIF_ICON | NIF_MESSAGE | NIF_TIP);
		nid_.hWnd = window_;
		nid_.hIcon = ::LoadIconW(nullptr, IDI_APPLICATION);
		nid_.uID = 1;
		nid_.uCallbackMessage = CWM_TASKTRAY;
		nid_.uTimeout = 10000;
		nid_.dwState = NIS_HIDDEN;
		::lstrcpyW(nid_.szTip, window_title_);

		::Shell_NotifyIconW(NIM_ADD, &nid_);

		return ::RegisterWindowMessageW(L"TaskbarCreated");
	}

	void main_window::tasktray_reload()
	{
		::Shell_NotifyIconW(NIM_DELETE, &nid_);
		::Shell_NotifyIconW(NIM_ADD, &nid_);
	}


	void main_window::tasktray_remove()
	{
		::Shell_NotifyIconW(NIM_DELETE, &nid_);
	}

	void main_window::menu_create()
	{
		HMENU menu;
		POINT pt;

		menu = ::CreatePopupMenu();

		MENUITEMINFO mi = { 0 };
		mi.cbSize = sizeof(MENUITEMINFO);
		WCHAR menustring[] = L"e&xit";

		// exit
		mi.fMask = MIIM_ID | MIIM_STRING;
		mi.wID = MID_EXIT;
		mi.dwTypeData = menustring;
		::InsertMenuItemW(menu, -1, TRUE, &mi);

		::GetCursorPos(&pt);
		::SetForegroundWindow(window_);
		::TrackPopupMenu(menu, TPM_BOTTOMALIGN, pt.x, pt.y, 0, window_, NULL);

		::DestroyMenu(menu);
	}

	std::wstring main_window::get_full_path(std::wstring path)
	{

		std::vector<WCHAR> fullpath_buffer(32768, L'\0');

		// モジュールインスタンスからEXEパスを取得
		auto len = ::GetModuleFileNameW(instance_, fullpath_buffer.data(), fullpath_buffer.size());

		// EXE部分を消す
		for (int i = len - 1; i > 0; i--) {
			if (fullpath_buffer.at(i) == L'\\') {
				fullpath_buffer.at(i) = L'\0';
				break;
			}
		}

		// bgm.wav を付与
		std::wstring full_path = fullpath_buffer.data();
		full_path += L"\\";
		full_path += path;

		return full_path;
	}

	LRESULT main_window::window_proc(UINT _message, WPARAM _wparam, LPARAM _lparam)
	{
		switch (_message)
		{
		case WM_CREATE:
		{
			// フック
			khook_.hook(window_);

			// タスクトレイ追加
			taskbar_created_ = tasktray_add();

			break;
		}

		case WM_DESTROY:
		{
			// タスクトレイ削除
			tasktray_remove();

			// アンフック
			khook_.unhook();

			::PostQuitMessage(0);
			return 0;
		}

		case CWM_GLOBAL_KEYDOWN:
		{
			DWORD vkcode = (DWORD)_wparam;

			break;
		}

		case CWM_GLOBAL_KEYUP:
		{
			DWORD vkcode = (DWORD)_wparam;
			if (vkcode == VK_F21) {
				bool state;
				if (volume_.toggle_mute(state))
				{
					if (state)
					{
						// ミュート
						::PlaySoundW(wav_mute_.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
					}
					else
					{
						// ミュート解除
						::PlaySoundW(wav_unmute_.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
					}
				}
			}
			else if (vkcode == VK_F24)
			{
				terminate_exe(L"r5apex.exe");
			}
			break;
		}

		case CWM_TASKTRAY:
			switch (_lparam)
			{
			case WM_RBUTTONDOWN:
				menu_create();
				break;
			}
			break;

		case WM_COMMAND:
			if (HIWORD(_wparam) == 0 && _lparam == 0)
			{
				WORD id = LOWORD(_wparam);
				if (id == MID_EXIT)
				{
					::DestroyWindow(window_);
				}
			}
			break;

		default:
			if (_message == taskbar_created_)
			{
				tasktray_reload();
			}
		}

		return ::DefWindowProcW(window_, _message, _wparam, _lparam);
	}

	LRESULT CALLBACK main_window::window_proc_common(HWND _window, UINT _message, WPARAM _wparam, LPARAM _lparam)
	{
		if (_message == WM_NCCREATE)
		{
			// createwindowで指定したポイントからインスタンスを取得
			auto cs = reinterpret_cast<CREATESTRUCTW*>(_lparam);
			auto instance = reinterpret_cast<main_window*>(cs->lpCreateParams);

			instance->window_ = _window;

			// USERDATAにポインタ格納
			::SetWindowLongPtrW(_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(instance));
		}

		// 既にデータが格納されていたらインスタンスのプロシージャを呼び出す
		if (auto ptr = reinterpret_cast<main_window*>(::GetWindowLongPtrW(_window, GWLP_USERDATA)))
		{
			return ptr->window_proc(_message, _wparam, _lparam);
		}

		return ::DefWindowProcW(_window, _message, _wparam, _lparam);
	}
}
