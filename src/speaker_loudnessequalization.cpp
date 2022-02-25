#include "speaker_loudnessequalization.hpp"

#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <propvarutil.h>
#include <Functiondiscoverykeys_devpkey.h>

#pragma comment(lib, "Propsys.lib")

const PROPERTYKEY PKEY_Realtek_LoudnessEqualization = { {0xE0A941A0, 0x88A2, 0x4df5, {0x8D, 0x6B, 0xDD, 0x20, 0xBB, 0x06, 0xE8, 0xFB}}, 4 }; // DEVPROP_TYPE_UINT32
interface DECLSPEC_UUID("568b9108-44bf-40b4-9006-86afe5b5a620") IPolicyConfig;
class DECLSPEC_UUID("294935CE-F637-4E7C-A41B-AB255460B862") CPolicyConfigClient;

interface IPolicyConfig : public IUnknown
{
public:

	virtual HRESULT GetMixFormat(
		__in PCWSTR,
		__out WAVEFORMATEX**
	);

	virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(
		__in PCWSTR,
		__in BOOL,
		__out WAVEFORMATEX**
	);

	virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(
		__in PCWSTR,
		__in WAVEFORMATEX*,
		__in WAVEFORMATEX*
	);

	virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(
		__in PCWSTR,
		__in BOOL,
		__out PINT64,
		__out PINT64
	);

	virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(
		__in PCWSTR,
		__in PINT64
	);

	virtual HRESULT STDMETHODCALLTYPE GetShareMode(
		__in PCWSTR,
		__out struct DeviceShareMode*
	);

	virtual HRESULT STDMETHODCALLTYPE SetShareMode(
		__in PCWSTR,
		__in struct DeviceShareMode*
	);

	virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(
		__in PCWSTR,
		__in BOOL,
		__in REFPROPERTYKEY,
		__out PROPVARIANT*
	);

	virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(
		__in PCWSTR,
		__in INT,
		__in REFPROPERTYKEY,
		__in REFPROPVARIANT
	);

	virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(
		__in PCWSTR wszDeviceId,
		__in ERole eRole
	);

	virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(
		__in PCWSTR,
		__in BOOL
	);
};

namespace app {
	speaker_loudnessequalization::speaker_loudnessequalization()
	{
		auto hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
	}

	speaker_loudnessequalization::~speaker_loudnessequalization()
	{
		::CoUninitialize();
	}

	bool speaker_loudnessequalization::toggle(bool &_state)
	{
		HRESULT hr;
		IMMDeviceEnumerator* enumrator = NULL;
		IMMDevice* device = NULL;
		IPolicyConfig* policy = NULL;
		LPWSTR device_id = NULL;
		PROPVARIANT v;
		bool rc = false;
		BOOL state = true;

		::PropVariantInit(&v);

		hr = ::CoCreateInstance(__uuidof(::MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&enumrator));
		if (FAILED(hr))
		{
			goto end;
		}

		// デフォルトのレンダーデバイスを取得
		hr = enumrator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
		if (FAILED(hr))
		{
			goto end;
		}

		// DEVICE ID取得
		hr = device->GetId(&device_id);
		if (hr != S_OK)
		{
			goto end;
		}
		
		// プロパティを変更するPolicyConfig取得
		hr = ::CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_ALL, IID_PPV_ARGS(&policy));
		if (FAILED(hr))
		{
			goto end;
		}

		// プロパティ取得
		hr = policy->GetPropertyValue(device_id, TRUE, PKEY_Realtek_LoudnessEqualization, &v);
		if (!SUCCEEDED(hr))
		{
			goto end;
		}

		// 値の確認
		if (v.vt != VT_UI4)
		{
			goto end;
		}

		// ON/OFF切り替え
		v.uintVal = v.uintVal == 0 ? 1 : 0;

		// プロパティ設定
		hr = policy->SetPropertyValue(device_id, TRUE, PKEY_Realtek_LoudnessEqualization, v);
		if (!SUCCEEDED(hr))
		{
			goto end;
		}
		
		// 返り値を設定
		_state = v.uintVal == 1;
		rc = true;

	end:
		if (policy)
			policy->Release();
		if (device_id)
			::CoTaskMemFree(device_id);
		if (device)
			device->Release();
		if (enumrator)
			enumrator->Release();

		::PropVariantClear(&v);

		return rc;
	}
}
