#include "StdAfx.h"
#include "WavePlayer.h"


WavePlayer::WavePlayer(LPDIRECTSOUND pDS)
{
	m_pDS= pDS;
	m_pDSBuffer = NULL;
	m_hmmio = NULL;
	m_pData = NULL;
	m_dwDataSize = 0;
	m_dwExtraAlloc = 0;
	m_pWf = NULL;
	m_PlayMode = Mode_Stop;
	//m_pDSBuffer8 = NULL;
}


WavePlayer::~WavePlayer(void)
{
	Release();
}

void WavePlayer::Play(CString strFilePath)
{
	if (m_hmmio == NULL){
		if (!LoadWaveFile(strFilePath)){
			return ;
		}
	}
	if (m_PlayMode != Mode_Playing)
		ToPlay();
	m_PlayMode = Mode_Playing;
}

void WavePlayer::Stop()
{
	if (m_pDSBuffer != NULL){
		m_pDSBuffer->Stop();
		m_pDSBuffer->SetCurrentPosition(0);
		m_PlayMode = Mode_Stop;
	}
}

void WavePlayer::Pause()
{
	if (m_pDSBuffer != NULL){
		m_pDSBuffer->Stop();
		m_PlayMode = Mode_Pause;
	}
}

void WavePlayer::Restart()
{
	Stop();
	if (m_pDSBuffer != NULL){
		m_pDSBuffer->Play(0, 0, 0);
		m_PlayMode = Mode_Playing;
	}
}


void WavePlayer::ToPlay()
{
	if (m_pDSBuffer != NULL){
		m_pDSBuffer->Stop();
		m_pDSBuffer->SetCurrentPosition(0);
		m_pDSBuffer->Play(0, 0, 0);
	}
}


BOOL WavePlayer::LoadWaveFile(CString strFilePath)
{
	LPTSTR lpstrFile = strFilePath.GetBuffer();
	m_hmmio = ::mmioOpen(lpstrFile, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (m_hmmio == NULL)
		return FALSE;
	WAVEFORMATEX* pwf = NULL;
	BYTE* pData = NULL;
	WORD dwExtraAlloc = 0;
	DWORD datasize = 0;
	try{
		MMCKINFO ckRiff;
		if (::mmioDescend(m_hmmio, &ckRiff, NULL, 0) != MMSYSERR_NOERROR){
			throw _T("throw find riff error");
		}
		if (ckRiff.ckid != FOURCC_RIFF || ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E')){
			throw _T("throw find wave chunk error");
		}
		MMCKINFO ckfmt;
		ckfmt.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if (::mmioDescend(m_hmmio, &ckfmt, &ckRiff, MMIO_FINDCHUNK) != MMSYSERR_NOERROR){
			throw _T("throw find fmt chunk error");
		}
		if (ckfmt.cksize < (DWORD)sizeof(PCMWAVEFORMAT)){
			throw _T("throw fmt chunk data error");
		}
		PCMWAVEFORMAT fmt;
		if (::mmioRead(m_hmmio, (HPSTR)&fmt, sizeof(fmt)) != sizeof(fmt)){
			throw _T("throw read fmt chunk error");
		}
		if (fmt.wf.wFormatTag == WAVE_FORMAT_PCM){
			dwExtraAlloc = 0;
		} else {
			if (::mmioRead(m_hmmio, (HPSTR)&dwExtraAlloc, sizeof(dwExtraAlloc)) != sizeof(dwExtraAlloc)){
				throw _T("throw read extra alloc error");
			}
		}
		pwf = (WAVEFORMATEX*)::VirtualAlloc(NULL, sizeof(WAVEFORMATEX) + dwExtraAlloc, MEM_COMMIT, PAGE_READWRITE);
		if (pwf == NULL){
			throw _T("throw VirtualAlloc pwf");
		}
		memcpy(pwf, &fmt, sizeof(fmt));
		pwf->cbSize = dwExtraAlloc;
		if (dwExtraAlloc != 0){
			if (::mmioRead(m_hmmio, (HPSTR)((BYTE*)(&pwf->cbSize) + dwExtraAlloc), sizeof(dwExtraAlloc)) != sizeof(dwExtraAlloc)){
				throw _T("read extradata error");
			}
		}
		if (::mmioAscend(m_hmmio, &ckfmt, 0) != MMSYSERR_NOERROR){
			throw _T("throw jump out of fmt chunk error");
		}
		if (::mmioSeek(m_hmmio, ckRiff.dwDataOffset + sizeof(FOURCC), SEEK_SET) == -1){
			throw _T("throw seek data chunk error");
		}
		MMCKINFO ckdata;
		ckdata.ckid = mmioFOURCC('d', 'a', 't', 'a');
		if (::mmioDescend(m_hmmio, &ckdata, &ckRiff, MMIO_FINDCHUNK) != MMSYSERR_NOERROR){
			throw _T("throw find data chunk error");
		}
		datasize = ckdata.cksize;
		pData = (BYTE*)::VirtualAlloc(NULL, datasize, MEM_COMMIT, PAGE_READWRITE);
		if (pData == NULL){
			throw _T("throw VirualAlloc error");
		}
		MMIOINFO mmioinfo;
		if (::mmioGetInfo(m_hmmio, &mmioinfo, 0) != MMSYSERR_NOERROR){
			throw _T("throw getinfo error");
		}
		ckdata.cksize = 0;
		for (DWORD tmp = 0; tmp < datasize; tmp++){
			if (mmioinfo.pchNext == mmioinfo.pchEndRead){
				if (::mmioAdvance(m_hmmio, &mmioinfo, MMIO_READ) != MMSYSERR_NOERROR){
					throw _T("throw read data into buffer error");
				}
				if (mmioinfo.pchNext == mmioinfo.pchEndRead){
					throw _T("throw read data error");
				}
			}
			*(pData + tmp) = *mmioinfo.pchNext;
			mmioinfo.pchNext++;
		}
		m_pData = pData;
		m_dwDataSize = datasize;
		m_pWf = pwf;
		m_dwExtraAlloc = dwExtraAlloc;
	} catch (...){
		if (m_hmmio != NULL){
			::mmioClose(m_hmmio, 0);
			m_hmmio = NULL;
		}
		if (pwf != NULL){
			::VirtualFree(pwf, sizeof(WAVEFORMATEX) + dwExtraAlloc, MEM_DECOMMIT);
			pwf = NULL;
		}
		if (pData != NULL){
			::VirtualFree(pData, datasize, MEM_DECOMMIT);
			pData = NULL;
		}
		return FALSE;
	}
	//创建辅助缓存对象
	return CreateDSBuffer();
}

BOOL WavePlayer::CreateDSBuffer()
{
	if (m_pWf == NULL || m_pData == NULL){
		return FALSE;
	}
	if (m_pDSBuffer != NULL){
		return FALSE;
	}
	DSBUFFERDESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = 0;
	desc.dwFlags |= DSBCAPS_STICKYFOCUS /*| DSBCAPS_GLOBALFOCUS*/ | DSBCAPS_GETCURRENTPOSITION2 | 0xE0;
	desc.dwBufferBytes = m_dwDataSize;
	desc.lpwfxFormat = m_pWf;
	
	HRESULT hr = m_pDS->CreateSoundBuffer(&desc, &m_pDSBuffer, NULL);
	if (FAILED(hr)){
		return FALSE;
	}
	//在高版本上可创建LPDIRECTSOUNDBUFFER8
	//hr = pDSBuffer->QueryInterface(IID_IDirectSound8, (LPVOID*)&m_pDSBuffer8);
	//if (FAILED(hr)){
	//	return FALSE;
	//}
	BYTE* pData1 = NULL, *pData2 = NULL;
	DWORD len1 = 0, len2 = 0;
	hr = m_pDSBuffer->Lock(0,m_dwDataSize, (void**)&pData1, &len1, (void**)&pData2, &len2, 0L);
	if (FAILED(hr)){
		return FALSE;
	}
	memcpy(pData1, m_pData, m_dwDataSize);
	hr = m_pDSBuffer->Unlock((void*)pData1, m_dwDataSize, NULL, 0);
	if (FAILED(hr)){
		return FALSE;
	}
	pData1 = NULL;
	hr = m_pDSBuffer->SetVolume(0);
	if (FAILED(hr)){
		return FALSE;
	}
	hr = m_pDSBuffer->SetPan(0);
	if (FAILED(hr)){
		return FALSE;
	}
	return TRUE;
}

void WavePlayer::Release()
{
	if (m_hmmio != NULL){
		::mmioClose(m_hmmio, 0);
		m_hmmio = NULL;
	}
	if (m_pWf != NULL){
		::VirtualFree(m_pWf, sizeof(WAVEFORMATEX) + m_dwExtraAlloc, MEM_DECOMMIT);
		m_pWf = NULL;
	}
	if (m_pData != NULL){
		::VirtualFree(m_pData, m_dwDataSize, MEM_DECOMMIT);
		m_pData = NULL;
	}
	if (m_pDSBuffer != NULL){
		m_pDSBuffer->Stop();
		m_pDSBuffer->Release();
		m_pDSBuffer = NULL;
	}
}

void WavePlayer::testNotify()
{
// 	if (m_hEvent == NULL){//在别的地方创建这个事件
// 		m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
// 		if (m_hEvent == NULL){
// 			return ;
// 		}
// 	}
	LPDIRECTSOUNDNOTIFY lpDSNotify;
	if (FAILED(m_pDSBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)&lpDSNotify)))
		return ;
	DSBPOSITIONNOTIFY notify;
	notify.dwOffset = DSBPN_OFFSETSTOP;
	notify.hEventNotify = m_hEvent;
	lpDSNotify->SetNotificationPositions(1, &notify);
	lpDSNotify->Release();
}

void WavePlayer::RestorBuffer()
{
	LPBYTE  pbData;
	LPBYTE  pbData2;
	DWORD   dwLen;
	DWORD   dwLen2;
	BOOL    bOK;
	HRESULT hr;

	bOK = FALSE;
	pbData = NULL;
	pbData2 = NULL;
	hr = m_pDSBuffer->Restore();

	if(hr == DS_OK) 
	{
		// Lock the buffer. 
		hr = m_pDSBuffer->Lock(0, m_dwDataSize, (void**)&pbData, 
			&dwLen, (void**)&pbData2, &dwLen2, 0);

		if(hr == DS_OK) 
		{
			ASSERT(pbData != NULL);
			ASSERT(m_pData != NULL);

			//Copy the buffer
			memcpy(pbData, m_pData, m_dwDataSize);

			//Unlock the buffer
			hr = m_pDSBuffer->Unlock(pbData, dwLen, NULL, 0);

			if(hr == DS_OK) 
			{
				//This is playing
				if (m_PlayMode == Mode_Playing){
					m_pDSBuffer->Play(0,0,0);
				}
				bOK = TRUE;
			}
		}

	}
}