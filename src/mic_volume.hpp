#pragma once

#include "common.hpp"

namespace app {

	class mic_volume
	{
	private:
	public:
		mic_volume();
		~mic_volume();

		// コピー不可
		mic_volume(const mic_volume&) = delete;
		mic_volume& operator = (const mic_volume&) = delete;
		// ムーブ不可
		mic_volume(mic_volume&&) = delete;
		mic_volume& operator = (mic_volume&&) = delete;

		bool toggle_mute(bool &);
	};
}
