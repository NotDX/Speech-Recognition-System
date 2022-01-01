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
    HWAVEIN hRecord_;            // ��Ƶ�豸���
    WAVEFORMATEX waveFormat_;    // Wave��ʽ����
	FILE *fpFile_;

    // ¼���ص�����
    static void CALLBACK WaveInProc
    (
        HWAVEIN hwi,            // ��Ƶ�豸���
        UINT    uMsg,           // ��Ϣ��ʶ
        DWORD   dwInstance,     // �û���������
        DWORD   dwParam1,       // ��Ϣ����
        DWORD   dwParam2        // ��Ϣ����
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
