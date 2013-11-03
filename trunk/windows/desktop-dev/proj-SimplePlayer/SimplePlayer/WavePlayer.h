#pragma once
class WavePlayer
{
public:
	//WavePlayer(){}
	WavePlayer(LPDIRECTSOUND pDS);
	~WavePlayer(void);

	enum MODE_PLAY{Mode_Playing = 1, Mode_Pause = 2, Mode_Stop = 3};

	void Play(CString strFilePath);
	void Stop();
	void Pause();
	void Restart();
	BOOL LoadWaveFile(CString strFilePath);

private:
	BOOL CreateDSBuffer();
	void Release();
	void ToPlay();

	void RestorBuffer();

	void testNotify();
private:
	LPDIRECTSOUND m_pDS;
	LPDIRECTSOUNDBUFFER m_pDSBuffer;
	//LPDIRECTSOUNDBUFFER8 m_pDSBuffer8;
	HMMIO m_hmmio;
	BYTE* m_pData;
	DWORD m_dwDataSize;
	WORD m_dwExtraAlloc;
	WAVEFORMATEX* m_pWf;
	MODE_PLAY m_PlayMode;
	HANDLE m_hEvent;
};

