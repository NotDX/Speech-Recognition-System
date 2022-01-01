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
    HWAVEIN hwi,        // 音频设备句柄
    UINT    uMsg,       // 消息标识
    DWORD   dwInstance, // 用户定义数据
    DWORD   dwParam1,   // 消息参数
    DWORD   dwParam2    // 消息参数
)
{
    // 过滤消息(只处理数据消息)
    if (uMsg != WIM_DATA) return;
	LPWAVEHDR lpHdr = (LPWAVEHDR)(dwParam1);
	
	if (lpHdr->dwBytesRecorded <= 0) 
	{
		return;
	}

    // 取得参数值
    WaveRecorder *pRecorder = (WaveRecorder *)(dwInstance);
//	if(pRecorder->fpFile_) fwrite(lpHdr->lpData, lpHdr->dwBytesRecorded, 1, pRecorder->fpFile_);

	SendOutNetwork(lpHdr->lpData,lpHdr->dwBytesRecorded);

    // 准备新缓冲
    HWAVEIN& hRecord = pRecorder->hRecord_;
    ::waveInUnprepareHeader(hRecord, lpHdr, sizeof(WAVEHDR));
    ::waveInPrepareHeader(hRecord, lpHdr, sizeof(WAVEHDR));
    ::waveInAddBuffer(hRecord, lpHdr, sizeof(WAVEHDR));
}

// 打开录音设备int WavePlayer::Open(int _block_size,int _block_count)
int WaveRecorder::Open(int _block_size,int _block_count)
{
    // 打开音频设备
    MMRESULT mmReturn = ::waveInOpen(&hRecord_,WAVE_MAPPER,&waveFormat_,(DWORD)(WaveRecorder::WaveInProc),
		(DWORD)(this),CALLBACK_FUNCTION);
    if (mmReturn != MMSYSERR_NOERROR)
        return mmReturn;
    
    // 加入缓冲
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

// 关闭录音设备
int WaveRecorder::Close()
{
    // 关闭设备
    return ::waveInClose(hRecord_);
}

// 开始
int WaveRecorder::Start()
{
    // 开始录音
    return ::waveInStart(hRecord_);
}

// 停止录音
int WaveRecorder::Stop()
{
    // 停止录音
    //return::waveInStop(hRecord_);
	return ::waveInReset(hRecord_);
}
//End of this file.
 