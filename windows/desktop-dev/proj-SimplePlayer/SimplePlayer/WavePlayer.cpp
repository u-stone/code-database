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
	m_bPlaying = FALSE;
}


WavePlayer::~WavePlayer(void)
{
	Release();
}

void WavePlayer::Play(CString strFilePath)
{
	if (m_bPlaying)
		return ;
	if (!LoadWaveFile(strFilePath)){
		 return ;
	}
	Begin();
	m_bPlaying = TRUE;
}

void WavePlayer::Begin()
{
	m_pDSBuffer->SetCurrentPosition(0);
	m_pDSBuffer->Play(0, 0, 0);
}


BOOL WavePlayer::LoadWaveFile(CString strFilePath)
{
	LPTSTR lpstrFile = strFilePath.GetBuffer();
	m_hmmio = ::mmioOpen(lpstrFile, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (m_hmmio == NULL)
		return FALSE;
	MMCKINFO ckRiff;
	if (::mmioDescend(m_hmmio, &ckRiff, NULL, 0) != MMSYSERR_NOERROR)
		return FALSE;
	if (ckRiff.ckid != FOURCC_RIFF || ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E'))
		return FALSE;
	MMCKINFO ckfmt;
	ckfmt.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (::mmioDescend(m_hmmio, &ckfmt, &ckRiff, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
		return FALSE;
	if (ckfmt.cksize < (DWORD)sizeof(PCMWAVEFORMAT)){
		return FALSE;
	}
	PCMWAVEFORMAT fmt;
	if (::mmioRead(m_hmmio, (HPSTR)&fmt, sizeof(fmt)) != sizeof(fmt)){
		return FALSE;
	}
	WORD dwExtraAlloc = 0;
	if (fmt.wf.wFormatTag == WAVE_FORMAT_PCM){
		dwExtraAlloc = 0;
	} else {
		if (::mmioRead(m_hmmio, (HPSTR)&dwExtraAlloc, sizeof(dwExtraAlloc)) != sizeof(dwExtraAlloc)){
			return FALSE;
		}
	}
	WAVEFORMATEX* pwf = (WAVEFORMATEX*)::VirtualAlloc(NULL, sizeof(WAVEFORMATEX) + dwExtraAlloc, MEM_COMMIT, PAGE_READWRITE);
	if (pwf == NULL){
		return FALSE;
	}
	memcpy(pwf, &fmt, sizeof(fmt));
	pwf->cbSize = dwExtraAlloc;
	if (dwExtraAlloc != 0){
		if (::mmioRead(m_hmmio, (HPSTR)((BYTE*)(&pwf->cbSize) + dwExtraAlloc), sizeof(dwExtraAlloc)) != sizeof(dwExtraAlloc)){
			return FALSE;
		}
	}
	if (::mmioAscend(m_hmmio, &ckfmt, 0) != MMSYSERR_NOERROR){
		return FALSE;
	}
	if (::mmioSeek(m_hmmio, ckRiff.dwDataOffset + sizeof(FOURCC), SEEK_SET) == -1){
		return FALSE;
	}
	MMCKINFO ckdata;
	ckdata.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (::mmioDescend(m_hmmio, &ckdata, &ckRiff, MMIO_FINDCHUNK) != MMSYSERR_NOERROR){
		return FALSE;
	}
	DWORD datasize = ckdata.cksize;
	BYTE* pData = (BYTE*)::VirtualAlloc(NULL, datasize, MEM_COMMIT, PAGE_READWRITE);
	if (pData == NULL){
		return FALSE;
	}
	MMIOINFO mmioinfo;
	if (::mmioGetInfo(m_hmmio, &mmioinfo, 0) != MMSYSERR_NOERROR){
		return FALSE;
	}
	ckdata.cksize = 0;
	for (DWORD tmp = 0; tmp < datasize; tmp++){
		if (mmioinfo.pchNext == mmioinfo.pchEndRead){
			if (::mmioAdvance(m_hmmio, &mmioinfo, MMIO_READ) != MMSYSERR_NOERROR){
				return FALSE;
			}
			if (mmioinfo.pchNext == mmioinfo.pchEndRead){
				return FALSE;
			}
		}
		*(pData + tmp) = *mmioinfo.pchNext;
		mmioinfo.pchNext++;
	}
	m_pData = pData;
	m_dwDataSize = datasize;
	m_pWf = pwf;
	m_dwExtraAlloc = dwExtraAlloc;
	
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
	desc.dwFlags |= DSBCAPS_STICKYFOCUS | DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2 | 0xE0;
	desc.dwBufferBytes = m_dwDataSize;
	desc.lpwfxFormat = m_pWf;
	
	HRESULT hr = m_pDS->CreateSoundBuffer(&desc, &m_pDSBuffer, NULL);
	if (FAILED(hr)){
		return FALSE;
	}
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
	if (m_pDSBuffer != NULL){
		m_pDSBuffer->Release();
		m_pDSBuffer = NULL;
	}
	if (m_pData != NULL){
		::VirtualFree(m_pData, m_dwDataSize, MEM_DECOMMIT);
		m_pData = NULL;
	}
	if (m_pWf != NULL){
		::VirtualFree(m_pWf, sizeof(WAVEFORMATEX) + m_dwExtraAlloc, MEM_DECOMMIT);
		m_pWf = NULL;
	}
	::mmioClose(m_hmmio, 0);
}
