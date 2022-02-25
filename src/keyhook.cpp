#include "keyhook.hpp"

#include <string>
#include <algorithm>

namespace {
	std::wstring get_device_name(HANDLE _handle)
	{
		// バッファサイズ確定
		UINT size = 0;
		if (::GetRawInputDeviceInfoW(_handle, RIDI_DEVICENAME, nullptr, &size) != 0) return L"";

		// データ読み出し
		std::vector<WCHAR> buf(size + 1, L'\0');
		if (::GetRawInputDeviceInfoW(_handle, RIDI_DEVICENAME, buf.data(), &size) == -1) return L"";
		return buf.data();
	}
}

namespace app {

	keyhook::keyhook()
		: window_(NULL)
		, mdevices_()
		, idevices_()
	{
	}

	keyhook::~keyhook()
	{
	}

	bool keyhook::hook(HWND _window)
	{
		window_ = _window;

		// RawInput開始
		RAWINPUTDEVICE dev = { 0 };
		dev.usUsagePage = 0x01;
		dev.usUsage = 0x06;
		dev.dwFlags = RIDEV_INPUTSINK;
		dev.hwndTarget = window_;
		return ::RegisterRawInputDevices(&dev, 1, sizeof(dev)) == TRUE;
	}

	bool keyhook::unhook()
	{
		// RawInput停止
		RAWINPUTDEVICE dev = { 0 };
		dev.usUsagePage = 0x01;
		dev.usUsage = 0x06;
		dev.dwFlags = RIDEV_REMOVE;
		dev.hwndTarget = NULL;
		return ::RegisterRawInputDevices(&dev, 1, sizeof(dev)) == TRUE;
	}

	bool keyhook::check_device(HANDLE _device)
	{
		if (_device == NULL) return false;

		// チェック済みの場合は結果を返す
		if (std::find(mdevices_.begin(), mdevices_.end(), _device) != mdevices_.end()) return true;
		if (std::find(idevices_.begin(), idevices_.end(), _device) != idevices_.end()) return false;

		// 新規デバイスの場合は、名前からマッチしてるか確認
		auto name = get_device_name(_device);
		if (name.find(L"VID_BC42") != std::wstring::npos &&
			name.find(L"PID_0003") != std::wstring::npos)
		{
			mdevices_.push_back(_device);
			return true;
		}

		idevices_.push_back(_device);
		return false;
	}

	void keyhook::proc(const RAWINPUT& _i)
	{
		_i.header.hDevice;
		const auto& k = _i.data.keyboard;
		if (k.VKey == VK_F21 ||
			k.VKey == VK_F22 ||
			k.VKey == VK_F23 ||
			k.VKey == VK_F24)
		{
			// meishi2ではない場合は無視
			if (!check_device(_i.header.hDevice)) return;

			if (k.Flags & RI_KEY_BREAK)
			{
				::PostMessageW(window_, CWM_GLOBAL_KEYUP, k.VKey, 0);
			}
			else
			{
				::PostMessageW(window_, CWM_GLOBAL_KEYDOWN, k.VKey, 0);
			}
		}
	}
}
