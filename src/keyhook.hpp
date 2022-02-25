#pragma once

#include "common.hpp"

#include <vector>

namespace app {

	class keyhook
	{
	private:
		HWND window_;
		std::vector<HANDLE> mdevices_;
		std::vector<HANDLE> idevices_;

		bool check_device(HANDLE);

	public:
		keyhook();
		~keyhook();

		// コピー不可
		keyhook(const keyhook&) = delete;
		keyhook& operator = (const keyhook&) = delete;
		// ムーブ不可
		keyhook(keyhook&&) = delete;
		keyhook& operator = (keyhook&&) = delete;

		bool hook(HWND);
		bool unhook();

		void proc(const RAWINPUT&);
	};
}
