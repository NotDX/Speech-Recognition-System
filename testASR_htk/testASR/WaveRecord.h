#if !defined(_WaveRecorder_H)
#define _WaveRecorder_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MMSYSTEM.H>
#pragma comment(lib,"winmm")

class WaveRecorder
{
public:
    HWAVEIN hRecord_;            // 音频设备句柄
    WAVEFORMATEX waveFormat_;    // Wave格式定义
	FILE *fpFile_;

    // 录音回调函数
    static void CALLBACK WaveInProc
    (
        HWAVEIN hwi,            // 音频设备句柄
        UINT    uMsg,           // 消息标识
        DWORD   dwInstance,     // 用户定义数据
        DWORD   dwParam1,       // 消息参数
        DWORD   dwParam2        // 消息参数
    );

public:
    WaveRecorder(const WAVEFORMATEX &waveFormatConfig);
    WaveRecorder(int Frequency = 16000, int BitsPerSample = 16, int Channels = 1);
    virtual ~WaveRecorder();

    virtual int Open(int _block_size,int _block_count);
    virtual int Close();

    virtual int Start();
    virtual int Stop();
};

#endif
