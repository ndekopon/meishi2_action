#include "common.hpp"

#include <combaseapi.h>

#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <audioclient.h>
#include <endpointvolume.h>

#include "mic_volume.hpp"

namespace app {
	mic_volume::mic_volume()
	{
		auto hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
	}

	mic_volume::~mic_volume()
	{
		::CoUninitialize();
	}

	bool mic_volume::toggle_mute(bool &_state)
	{
		HRESULT hr;
		IMMDeviceEnumerator* enumrator = NULL;
		IMMDevice* device = NULL;
		IAudioEndpointVolume* volume = NULL;
		bool rc = false;
		BOOL state = true;

		hr = ::CoCreateInstance(__uuidof(::MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumrator));
		if (FAILED(hr)) {
			enumrator = NULL;
			goto end;
		}

		// デフォルトのキャプチャデバイスを取得
		hr = enumrator->GetDefaultAudioEndpoint(eCapture, eConsole, &device);
		if (FAILED(hr)) {
			device = NULL;
			goto end;
		}

		// エンドポイントボリュームを取得
		hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&volume);
		if (FAILED(hr)) {
			volume = NULL;
			goto end;
		}

		// ミュート状態を取得
		hr = volume->GetMute(&state);
		if (FAILED(hr))
		{
			goto end;
		}

		// ミュート状態を設定
		hr = volume->SetMute(state == TRUE ? FALSE : TRUE, NULL);
		if (SUCCEEDED(hr))
		{
			_state = state != TRUE;
			rc = true;
		}

	end:
		if (volume)
			volume->Release();
		if (device)
			device->Release();
		if (enumrator)
			enumrator->Release();

		return rc;
	}
}
