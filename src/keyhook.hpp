#pragma once

#include "common.hpp"

namespace app {

	class keyhook
	{
	private:

	public:
		keyhook();
		~keyhook();

		// コピー不可
		keyhook(const keyhook&) = delete;
		keyhook& operator = (const keyhook&) = delete;
		// ムーブ不可
		keyhook(keyhook&&) = delete;
		keyhook& operator = (keyhook&&) = delete;

		bool hook(HINSTANCE, HWND);
		bool unhook();
	};
}
