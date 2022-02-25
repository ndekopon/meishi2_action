#pragma once

#include "common.hpp"

namespace app {

	class speaker_loudnessequalization
	{
	private:
	public:
		speaker_loudnessequalization();
		~speaker_loudnessequalization();

		// コピー不可
		speaker_loudnessequalization(const speaker_loudnessequalization&) = delete;
		speaker_loudnessequalization& operator = (const speaker_loudnessequalization&) = delete;
		// ムーブ不可
		speaker_loudnessequalization(speaker_loudnessequalization&&) = delete;
		speaker_loudnessequalization& operator = (speaker_loudnessequalization&&) = delete;

		bool toggle(bool &);
	};
}
