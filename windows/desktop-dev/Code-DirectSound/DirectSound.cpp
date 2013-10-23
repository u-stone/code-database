// DirectSound.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "dsound.h"
#include  <iostream>
using namespace std;
//1. 遍历声音设备
//

int _tmain(int argc, _TCHAR* argv[])
{
	LPDIRECTSOUND8 lpsd;
	//DirectSound基于COM,but you do not have to initilize COM, if you don't use effective DMOs
	//HRESULT hr = DirectSoundCreate8(NULL, &lpsd, NULL);
	//if (FAILED(hr)){
	//	cout << "DirectSound Create failed" << endl;
	//	return 0;
	//}
    hr = CoInitializeEx(NULL, 0);
	if (FAILED(hRes)){
		cout << "CoInitializeEx failed" << endl;
		return 0;
	}
	hr = CoCreateInstanceEx(&CLSID_DirectSound8, 
		NULL,
		CLSTX_INPROC_SERVER,
		IID_IDirectSound8,
		(LPVOID*)&lpsd);
	if (FAILED(hr)){
		cout << "CoCreateInstanceEx failed" << endl;
		return 0;
	}
	hr = lpsd->Initialize(NULL);
	if (FAILED(hr)){
		cout << "DirectSound Device Initialize failed" << endl;
		return 0;
	}
	//不设置这个，没有声音
	hr = lpsd->SetCooperativeLevel(NULL, DSSCL_PRIORITY);
	if (FAILED(hr)){
		cout << "SetCooperativeLevel Failed" << endl;
		return 0;
	}
	DSCAPS dscaps;
	dscaps.dwSize = sizeof(DSCAPS);
	hr = lpsd->GetCaps(&dscaps);//得到设备的相关参数，这一步一般不需要做
	if (FAILED(hr)){
		cout << "GetCaps failed" << endl;
		return 0;
	}
	//创建二级缓存，这个控制声音从源到目的地, source sound can come from synthesizer,another buffer,a wav file,resource
	//CreateBaseBuffer(lpsd, );

	CoUninitialize();
	return 0;
}

//get the DIRECTSOUNDBUFFER8 interface
HRESULT CreateBaseBuffer(LPDIRECTSOUND8 lpDirectSound, LPDIRECTSOUNDBUFFER8 * ppDsb8){
	WAVEFORMATEX wfx;
	DSBUFFERDESC dsbdesc;
	LPDIRECTSOUNDBUFFER pDsb = NULL;
	HRESULT hr;

	//set up WAV buffer format
	memset(&wfx, 0, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = 22050;
	wfx.nBlockAlign = 4;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.wBitsPerSample = 16;

	//set up DSBUFFERDESC structure
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	//the rule of using following flags is that use only what you will control
	dsbdesc.dwFlags = 
		DSBCAPS_CTRLPAN | 
		DSBCAPS_CTRLVOLUME |   //IDirectSound::SetVolume() is success only if this flag was set
		DSBCAPS_CTRLFREQUENCY | 
		DSBCAPS_GLOBALFOCUS; //ensure that even if the window is in the backgroud,that mean you can hear the sound when you lose the focus of the window
	dsbdesc.dwBufferBytes = 3 * wfx.nAvgBytesPerSec;//create a buffer that is big enough to hold 3 seconds of streaming data
	dsbdesc.lpwfxFormat = &wfx;//if the was not specified, ds will place it in hardware-controlled memory.
	//you can check the location of the buffer by using GetCaps() function, 
	//create buffer
	hr = lpDirectSound->CreateSoundBuffer(&dsbdesc, &pDsb, NULL);
	if (SUCCEEDED(hr)){
		hr = pDsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)ppDsb8);
		pDsb->Release();
	}
	//the created buffer owned by device object,and released when device object release the resources
	//you can also create multiple buffer by using function IDirectSound8::DuplicateSoundBuffer(),but the memory is share with original buffer,so if one buffer changed,others will be reflected.

	return hr;
}

//static buffer is created and managed just like streaming buffer,but static buffer is filled once and played,streaming buffer is filled and played contantly
void HowToUseStaticBuffer(IDirectSoundBuffer8* lpdsbStatic, LPVOID pbData)
{
	//to load data into static buffer
	/*
	1. IDirectSoundBuffer8::Lock() to lock the buffer, you should set offset of buffer you want to use,and get the start address
	2. use a standard memory-copy routine to write audio data into buffer
	3. IDirectSoundBuffer8::Unlock() to unlock the buffer
	*/
	LPVOID lpvWrite;
    DWORD dwLength;
	if (DS_OK == lpdsbStatic->Lock(0, 0, &lpvWrite, &dwLength, NULL, NULL,DSBLOCK_ENTIREBUFFER)){
		memcpy(lpvWrite, pbData, dwLength);
		lpdsbStatic->Unlock(lpvWrite, dwLength, NULL, 0);
	}else{
		//ErrorHandle();
	}
	//To play the buffer
	lpdsbStatic->SetCurrentPosition(0);//ensure we start from the begining
	HRESULT hr = lpdsbStatic->Play(0, 0, 0);
	if (FAILED(hr)){
		//ErrorHandle()
	}
	//as there is no setting DSBPLAY_LOOPING flag. buffer automatically stops when it reaches the end
	//you can stop the buffer by using IDirectSoundBuffer8::Stop()
}

BOOL HowToUseStreamBuffer(LPDIRECTSOUNDBUFFER8 lpDsb, DWORD dwOffset, LPBYTE lpbSoundData, DWORD dwSoundBytes)
{
	LPVOID lpvPtr1;
	DWORD dwBytes1;
	LPVOID lpvPtr2;
	DWORD dwBytes2;
	HRESULT hr;

	//获得锁定内存的首地址
	hr = lpDsb->Lock(dwOffset, dwSoundBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
	//如果缓存丢失，要重新装载并重试锁定
	if (DSERR_BUFFERLOST == hr){
		lpDsb->Restore();
		hr = lpDsb->Lock(dwOffset, dwSoundBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
	}
	if (SUCCEEDED(hr)){
		CopyMemory(lpvPtr1, lpbSoundData, dwBytes1);
		if (NULL != lpvPtr2){
			CopyMemory(lpvPtr2, lpbSoundData+dwBytes1, dwBytes2);
		}
		hr = lpDsb->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
		if (SUCCEEDED(hr)){
			return TRUE;
		}
	}
	//Lock, Unlock, or Restore failed
	return FALSE;
}


//设置播放通知，一般用处有二：
//1. 通知程序写入下一段声音数据
//2. 通知做某一个操作
HRESULT SetStopNotification(HANDLE hEvent, LPDIRECTSOUNDBUFFER8 lpDsbSecondary){
	LPDIRECTSOUNDNOTIFY8 lpDsNotify;
	DSBPOSITIONNOTIFY PositionNotify;
	HRESULT hr;

	if (SUCCEEDED(hr = 
		lpDsbSecondary->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)lpDsNotify)
		)){
	        PositionNotify.dwOffset = DSBPN_OFFSETSTOP;
			PositionNotify.hEventNotify = hEvent;
			hr = lpDsNotify->SetNotificationPositions(1, &PositionNotify);
			lpDsNotify->Release();
	}
	return hr;
}


//多通道高分辨率WAV文件


//计算WAV文件播放时间，这个时间由数据大小和格式共同决定
DWORD GetSoundLength(LPSTR strFileName){
	CWaveFile* pWav;
	DWORD dwLen = 0;
	DWORD dwSize;
	WAVEFORMATEX* pwfx;
	
	pWav = new CWaveFile();
	if (SUCCEEDED(pWav->Open(strFileName, NULL, WAVEFILE_READ)))){
		pwfx = pWav->GetFormat();
		dwSize = pWav->GetSize();
		dwLen = (DWORD)(1000 * dwSize / pwfx->nAvgBytesPerSec);
		pWav->Close();
	}
	if (pWav)
		delete pWav;
	return dwLen;
}


//获得3D收听对象
HRESULT get3DListener(LPDIRECTSOUND8 lpds, LPDIRECTSOUND3DBUFFER8* ppListener){
	DSBUFFERDESC dsbd;
	LPDIRECTSOUNDBUFFER lpdsbPrimary;
	LPDIRECTSOUND3DLISTENER lp3DListener = NULL;
	HRESULT hr;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	if (SUCCEEDED(hr = lpds->CreateSoundBuffer(&dsbd, &lpdsbPrimary, NULL))){
		hr = lpdsbPrimary->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)ppListener);
		lpdsbPrimary->Release();
	}
	return hr;
}


//设置声音特效
HRESULT SetEcho(LPDIRECTSOUNDBUFFER8 pDSBuffer){
	HRESULT hr;
	DWORD  dwResult[1];

	DSEFFECTDESC dsEffect;
	memset(&dsEffect, 0, sizeof(DSEFFECTDESC));
	dsEffect.dwSize = sizeof(DSEFFECTDESC);
	dsEffect.dwFlags = 0;
	dsEffect.guidDSFXClass = GUID_DSFX_STANDARD_ECHO;

	if (SUCCEEDED(hr = pDSBuffer->SetFX(1, &dsEffect, dwResult))){
		switch (dwResult[0])
		{
		case DSFXR_LOCHARDWARE:
			cout << "Effect was placed in hardware" << endl;
			break;
		case DSFXR_LOCSOFTWARE:
			cout << "Effect was placed in software" << endl;
			break;
		case DSFXR_UNALLOCATED:
			cout << "Effect was not allocated in hardware or software" << endl;
			break;
		}
	}
	return hr;
}


HRESULT SetCapturenotification(LPDIRECTSOUNDCAPTUREBUFFER8 pDSCB){
#define cEvent 3
	LPDIRECTSOUNDNOTIFY8 pDSBNotify;
	WAVEFORMATEX wfx;
	HANDLE		rghEvent[cEvent];
	DSBPOSITIONNOTIFY rgdsbpn[cEvent];
	HRESULT hr;

	if (NULL == pDSCB) return E_INVALIDARG;
	if (FAILED(hr = pDSCB->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&pDSBNotify))){
		return hr;
	}
	if (FAILED(hr = pDSCB->GetFormat(&wfx, sizeof(WAVEFORMATEX), NULL)))
		return hr;
	for (int i = 0;  i< cEvent; ++i){
		rghEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (rghEvent[i] == NULL){
			hr = GetLastError();
			return hr;
		}
	}
	rgdsbpn[0].dwOffset = wfx.nAvgBytesPerSec/2 - 1;
	rgdsbpn[0].hEventNotify = rghEvent[0];

	rgdsbpn[1].dwOffset = wfx.nAvgBytesPerSec - 1;
	rgdsbpn[1].hEventNotify = rghEvent[1];

	rgdsbpn[2].dwOffset = DSBPN_OFFSETSTOP;
	rgdsbpn[2].hEventNotify = rghEvent[2];

	hr = pDSBNotify->SetNotificationPositions(cEvent, rgdsbpn);
	pDSBNotify->Release();
	return hr;
}


BOOL ApplicationCreateWritePrimaryBuffer(
	LPDIRECTSOUND8 lpDirectSound, 
	LPDIRECTSOUNDBUFFER8 *lplpDsb,
	LPDWORD lpdwBufferSize,
	HWND hwnd
	){
		DSBUFFERDESC dsbdesc;
		DSBCAPS dsbcaps;
		HRESULT hr;
		WAVEFORMATEX wf;

		memset(&wf, 0, sizeof(WAVEFORMATEX));
		wf.wFormatTag = WAVE_FORMAT_PCM;
		wf.nChannels = 2;
		wf.nSamplesPerSec = 22050;
		wf.nBlockAlign = 4;
		wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
		wf.wBitsPerSample = 16;

		memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
		dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
		//缓存大小由声音硬件返回
		dsbdesc.dwBufferBytes = 0;
		dsbdesc.lpwfxFormat = NULL;//对于主缓存来说必须是NULL

		//得到写数据协作权限
		hr = lpDirectSound->SetCooperativeLevel(hwnd, DSSCL_WRITEPRIMARY);
		if (SUCCEEDED(hr)){
			hr = lpDirectSound->CreateSoundBuffer(&dsbdesc, lplpDsb, NULL);
			if (SUCCEEDED(hr)){
				hr = (*lplpDsb)->SetFormat(&wf);
				if (SUCCEEDED(hr)){
					dsbcaps.dwSize = sizeof(DSBCAPS);
					(*lplpDsb)->GetCaps(&dsbcaps);
					*lpdwBufferSize = dsbcaps.dwBufferBytes;
					return TRUE;
				}
			}
		}
		//失败
		*lplpDsb = NULL;
		*lpdwBufferSize = 0;
		return FALSE;
}

struct APPSTREAMINFO 
{
};
BOOL AppMixintoPrimaryBuffer(
	APPSTREAMINFO * lpAppStreamInfo,
	LPDIRECTSOUNDBUFFER lpDsbPrimary,
	DWORD dwDataBytes,
	DWORD dwOldPos,
	LPDWORD lpdwNewPos
	){
		LPVOID lpvPtr1;
		DWORD dwBytes1;
		LPVOID lpvPtr2;
		DWORD dwBytes2;
		HRESULT hr;
		//得到写指针
		hr = lpDsbPrimary->Lock(dwOld, dwDataBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
		if (DSERR_BUFFERLOST == hr)//需要重新装载并重试锁定
		{
			lpDsbPrimary->Restore();
			hr = lpDsbPrimary->Lock(dwOld, dwDataBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
		}
		if (SUCCEEDED(hr)){
			CustomMixer(lpAppStreamInfo, lpvPtr1, dwBytes1);
			*lpdwNewPos = dwOldPos + dwBytes1;
			if (NULL != lpvPtr2){
				CustomMixer(lpAppStreamInfo, lpvPtr2, dwBytes2);
				*lpdwNewPos = dwBytes2;//
			}
			hr = lpDsbPrimary->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
			if (SUCCEEDED(hr)){
				return TRUE;
			}
		}
		return FALSE;
}