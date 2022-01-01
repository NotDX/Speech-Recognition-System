//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include<fcntl.h>
#include<io.h>
#include <sys/timeb.h>
#include <stdio.h>
#include "WaveRecord.h"

extern int SendOutNetwork(char*buff,int size);
WaveRecorder::WaveRecorder(const WAVEFORMATEX &waveFormatConfig)
{
    waveFormat_ = waveFormatConfig;
	fpFile_ = NULL;
}

WaveRecorder::WaveRecorder(int Frequency, int BitsPerSample, int Channels)
{
    memset(&waveFormat_, 0x00, sizeof(WAVEFORMATEX));
    waveFormat_.wFormatTag = WAVE_FORMAT_PCM;		//WAVE_FORMAT_ALAW;//WAVE_FORMAT_PCM;
//	if(BitsPerSample==8) waveFormat_.wFormatTag=WAVE_FORMAT_ALAW;
    waveFormat_.nSamplesPerSec = Frequency;
    waveFormat_.wBitsPerSample = BitsPerSample;
    waveFormat_.nChannels = Channels;
    waveFormat_.nBlockAlign = waveFormat_.nChannels * waveFormat_.wBitsPerSample / 8;
    waveFormat_.nAvgBytesPerSec = waveFormat_.nSamplesPerSec * waveFormat_.nBlockAlign;
    waveFormat_.cbSize = 0;
    
	//fpFile_ = fopen("temp.pcm", "wb");
}

WaveRecorder::~WaveRecorder()
{
	if(fpFile_) fclose(fpFile_);
	fpFile_=NULL;
}

void CALLBACK WaveRecorder::WaveInProc
(
    HWAVEIN hwi,        // ��Ƶ�豸���
    UINT    uMsg,       // ��Ϣ��ʶ
    DWORD   dwInstance, // �û���������
    DWORD   dwParam1,   // ��Ϣ����
    DWORD   dwParam2    // ��Ϣ����
)
{
    // ������Ϣ(ֻ����������Ϣ)
    if (uMsg != WIM_DATA) return;
	LPWAVEHDR lpHdr = (LPWAVEHDR)(dwParam1);
	
	if (lpHdr->dwBytesRecorded <= 0) 
	{
		return;
	}

    // ȡ�ò���ֵ
    WaveRecorder *pRecorder = (WaveRecorder *)(dwInstance);
//	if(pRecorder->fpFile_) fwrite(lpHdr->lpData, lpHdr->dwBytesRecorded, 1, pRecorder->fpFile_);

	SendOutNetwork(lpHdr->lpData,lpHdr->dwBytesRecorded);

    // ׼���»���
    HWAVEIN& hRecord = pRecorder->hRecord_;
    ::waveInUnprepareHeader(hRecord, lpHdr, sizeof(WAVEHDR));
    ::waveInPrepareHeader(hRecord, lpHdr, sizeof(WAVEHDR));
    ::waveInAddBuffer(hRecord, lpHdr, sizeof(WAVEHDR));
}

// ��¼���豸int WavePlayer::Open(int _block_size,int _block_count)
int WaveRecorder::Open(int _block_size,int _block_count)
{
    // ����Ƶ�豸
    MMRESULT mmReturn = ::waveInOpen(&hRecord_,WAVE_MAPPER,&waveFormat_,(DWORD)(WaveRecorder::WaveInProc),
		(DWORD)(this),CALLBACK_FUNCTION);
    if (mmReturn != MMSYSERR_NOERROR)
        return mmReturn;
    
    // ���뻺��
    for (int i = 0; i < _block_count; ++i)
    {
        LPWAVEHDR lpHdr = new WAVEHDR;
        memset(lpHdr, 0x00, sizeof(WAVEHDR));
        
        lpHdr->lpData = new char[_block_size];
        lpHdr->dwBufferLength = _block_size;
        
        MMRESULT mmReturn = ::waveInPrepareHeader(hRecord_, lpHdr, sizeof(WAVEHDR));
        if (mmReturn != MMSYSERR_NOERROR)
            return mmReturn;
        
        mmReturn = ::waveInAddBuffer(hRecord_, lpHdr, sizeof(WAVEHDR));
        if (mmReturn != MMSYSERR_NOERROR)
            return mmReturn;
    }

    return 0;
}

// �ر�¼���豸
int WaveRecorder::Close()
{
    // �ر��豸
    return ::waveInClose(hRecord_);
}

// ��ʼ
int WaveRecorder::Start()
{
    // ��ʼ¼��
    return ::waveInStart(hRecord_);
}

// ֹͣ¼��
int WaveRecorder::Stop()
{
    // ֹͣ¼��
    //return::waveInStop(hRecord_);
	return ::waveInReset(hRecord_);
}
//End of this file.
 