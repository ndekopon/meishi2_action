#pragma once

#include "common.hpp"

#include "keyhook.hpp"
#include "mic_volume.hpp"

#include <string>

namespace app {

	class main_window
	{
	private:
		const HINSTANCE instance_;
		HWND window_;
		size_t width_;
		size_t height_;
		mic_volume volume_;
		keyhook khook_;
		NOTIFYICONDATAW nid_;
		UINT taskbar_created_;
		std::wstring wav_mute_;
		std::wstring wav_unmute_;

		static const wchar_t* window_class_;
		static const wchar_t* window_title_;
		static const wchar_t* window_mutex_;

		void disable_ime();
		void set_dpi_awareness();
		ATOM register_window_class();
		bool create_window();
		UINT tasktray_add();
		void tasktray_reload();
		void tasktray_remove();
		void menu_create();
		std::wstring get_full_path(std::wstring);

		LRESULT window_proc(UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK window_proc_common(HWND, UINT, WPARAM, LPARAM);

	public:
		main_window(HINSTANCE);
		~main_window();

		// コピー不可
		main_window(const main_window&) = delete;
		main_window& operator = (const main_window&) = delete;
		// ムーブ不可
		main_window(main_window&&) = delete;
		main_window& operator = (main_window&&) = delete;

		bool init();
		int  loop();
	};
}
