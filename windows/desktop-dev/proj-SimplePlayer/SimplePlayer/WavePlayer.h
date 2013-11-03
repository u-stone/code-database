#pragma once
class WavePlayer
{
public:
	//WavePlayer(){}
	WavePlayer(LPDIRECTSOUND pDS);
	~WavePlayer(void);

	void Play(CString strFilePath);
	BOOL LoadWaveFile(CString strFilePath);

private:
	BOOL CreateDSBuffer();
	void Release();
	void Begin();
private:
	LPDIRECTSOUND m_pDS;
	LPDIRECTSOUNDBUFFER m_pDSBuffer;
	HMMIO m_hmmio;
	BYTE* m_pData;
	DWORD m_dwDataSize;
	WORD m_dwExtraAlloc;
	WAVEFORMATEX* m_pWf;
	BOOL m_bPlaying;
};

