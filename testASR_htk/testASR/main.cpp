#include <iostream>
//#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include<fcntl.h>
#include<io.h>
#include <sys/timeb.h>
#include<conio.h>
#include <time.h>
#include <TCHAR.H>  

#include "mscomm.h"
#include "WaveRecord.h"
#include "ReadWavBuf.h"
#include "FileRead.h"
#include "tsASR.h"


#include <fstream>
#include <string>
#pragma comment(lib,"tsASR.lib")

using namespace std;

#define MAX_LEN 1024
#define MAX_UTT_LEN 160000  //10s*SAMPLERATE

DWORD WINAPI thread_ASR(LPVOID lpParamter);
HANDLE mutex_buffer;
short* pWavBuffer;
unsigned long pWavBufferLen;
int buffer_ok = 0;

HANDLE hCom;
bool doCom = false;
bool showComInfo = false;
CMSComm	m_Com;
string ComStr = "COM4";
int m_nInputMode = 1;			//传输模式：ASCII模式还是二进制模式
int m_nPort = 0;				//端口号
int BaudRate = 9600;          //波特率
int ByteSize = 8;           //位数 
string strSendString;       //发送信息
CString m_strSettings;		//串口设置
CString m_strSendString;

WaveRecorder* pRecorder = NULL;
VOICE_HANDLE* voice;
Handle tsASR, hASR = 1;

float scoreASR, confidence;
int numResult;
char* text;

int logo_cache = 1, min_voc_eng = 400;
int unrecorded_times = 0;
int recognized_times = 0;
int nospeak = 0;
int NoSpeak_Time = 300;
float confidence_thresh = 5;
int WakeUp_Times = 1;
bool recorded = false;
bool doWakeUp = false;
bool WakeUp = false;
string WakeUp_word = "ding dong ding dong";

static  CRITICAL_SECTION CriticalSection;
void  P()
{
	EnterCriticalSection(&CriticalSection);
}
void  V()
{
	LeaveCriticalSection(&CriticalSection);
}
void  Init_Lock()
{
	InitializeCriticalSection(&CriticalSection);
}
void  Delete_Lock()
{
	DeleteCriticalSection(&CriticalSection);
}

void trim(string& str)
{
	str.erase(str.find_last_not_of(' ') + 1, string::npos);
	str.erase(0, str.find_first_not_of(' '));
}

void trim_filler(string& str)
{
	int tag = str.find("filler");
	while (tag >= 0)
	{
		str.erase(tag, 8); //filler1,filler2,...	
		trim(str);
		tag = str.find("filler");
	}
	//str.erase(0, str.find_first_not_of("filler"));
}

void operate(string a)
{
	ofstream ofs;
	ofs.open("text.txt", ios::out);
	ofs << a << endl;
	ofs.close();
}
void operate2(string b)
{
	ofstream ofs;
	ofs.open("text.txt", ios::app);
	ofs << b << endl;
	ofs.close();
}
/**
  serial read
  @param Buf:data buf
  @param size:
  @return The len of read
*/
int Serial_read(void* OutBuf, int size)
{
	DWORD cnt = 0;
	ReadFile(hCom, OutBuf, size, &cnt, 0);
	return cnt;

}

/**
  serial write
  @param Buf:data buf
  @param size:bytes of Buf
  @return The len of writen
*/
int Serial_write(const void* Buf, int size)
{
	DWORD dw;
	if (showComInfo)
		printf("Send COM%d info: %s\n", m_nPort, Buf);
	WriteFile(hCom, Buf, size, &dw, NULL);
	//WriteFile(hCom,"*ag$",sizeof("*ag$"),&dw,NULL);
	return dw;
}

/**
  serial close
*/
void Serial_close(void)
{
	CloseHandle(hCom);
}

int DetectPort(string ComStr)
{
	char chrCom[100];
	int i;

	sprintf(chrCom, "%s", ComStr.c_str());
	int ReadN;
	ReadN = ComStr.length(); //fread(ComStr,1,100,fpConfig);
	for (i = 0; i < ReadN; i++)
	{
		if (chrCom[i] == ' ' || chrCom[i] == '\x0d' || chrCom[i] == '\x0a')
		{
			chrCom[i] = 0;
			break;
		}
	}
	if (i == ReadN)
		chrCom[i] = 0;
	//fclose(fpConfig);

	char* ComNumStr = chrCom;
	while (ComNumStr < chrCom + 100 && !(*ComNumStr >= '0' && *ComNumStr <= '9'))
		ComNumStr++;
	m_nPort = atoi(ComNumStr);
	if (showComInfo)
		printf("获取串口号：%d\n", m_nPort);

	//HANDLE hCom; 

	CString strCom;
	strCom.Format("COM%d", m_nPort);
	DCB dcb = { 0 };
	hCom = CreateFile(strCom,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,//FILE_FLAG_OVERLAPPED,   //同步方式 或 重叠方式  
		0
	);

	//hCom = CreateFile(strCom, 0, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);   
	if (INVALID_HANDLE_VALUE != hCom)
	{
		if (showComInfo)
			printf("%s: 此串口有效!\n", ComStr.c_str());
	}
	else
	{
		CloseHandle(hCom);
		if (showComInfo)
			printf("%s: 此串口无效!\n", ComStr.c_str());
		//exit(-1);
	}

	return m_nPort;
}

int InitMicPlayer()
{
	static int started;
	int res;
	if (started == 0)
	{
		char tmp[1024];

		GetPrivateProfileString("SET", "LOGO_CACHE", "100", tmp, 20, "./set.ini");
		logo_cache = atoi(tmp);

		GetPrivateProfileString("SET", "MIN_VOC_ENG", "300", tmp, 20, "./set.ini");
		min_voc_eng = atoi(tmp);

		GetPrivateProfileString("SET", "doWakeUp", "true", tmp, 20, "./set.ini");
		if (stricmp(tmp, "true") == 0) doWakeUp = true;
		else doWakeUp = false;

		GetPrivateProfileString("SET", "WakeUp_Word", "ding dong ding dong", tmp, 20, "./set.ini");
		WakeUp_word = tmp;
		trim(WakeUp_word);
		//WakeUp_word+=" "; //space

		GetPrivateProfileString("SET", "NoSpeak_Time", "3", tmp, 20, "./set.ini");
		NoSpeak_Time = atoi(tmp) * 10; //10*100ms

		GetPrivateProfileString("SET", "Confidence_Thresh", "5", tmp, 20, "./set.ini");
		confidence_thresh = atof(tmp);

		started = 1;
		pRecorder = new WaveRecorder(SAMPLERATE, 16);//pcm16 录音
		res = pRecorder->Open(3200, 100);		//100ms
		if (pRecorder) res = pRecorder->Start();

		//----------------------串口---------------------------------------
		GetPrivateProfileString("COM", "doCom", "false", tmp, 20, "./set.ini");
		if (stricmp(tmp, "true") == 0) doCom = true;
		else doCom = false;

		GetPrivateProfileString("COM", "ComStr", "COM3", tmp, 20, "./set.ini");
		ComStr = tmp;

		GetPrivateProfileString("COM", "InputMode", "1", tmp, 20, "./set.ini");
		m_nInputMode = atoi(tmp);

		string strBaudRate;

		GetPrivateProfileString("COM", "BaudRate", "9600", tmp, 20, "./set.ini");
		strBaudRate = tmp;
		BaudRate = atoi(tmp);

		GetPrivateProfileString("COM", "ByteSize", "8", tmp, 20, "./set.ini");
		ByteSize = atoi(tmp);

		GetPrivateProfileString("COM", "strSendString", "*ag$", tmp, 20, "./set.ini");
		strSendString = tmp;

		GetPrivateProfileString("COM", "showComInfo", "false", tmp, 20, "./set.ini");
		if (stricmp(tmp, "true") == 0) showComInfo = true;
		else showComInfo = false;

		if (doCom)
		{
			m_nPort = DetectPort(ComStr);
		}

		char strPort[10];
		itoa(m_nPort, strPort, 10);
		m_strSettings = strBaudRate.c_str();
		m_strSettings = m_strSettings + _T(",n,8,1");	//串口设置				

		m_strSendString = strSendString.c_str();  //_T("CA 5D 01 03 2B");	//发送命令

		DCB dcb = { 0 };
		dcb.DCBlength = sizeof(DCB);

		if (!GetCommState(hCom, &dcb))
		{
			DWORD dwError = GetLastError();
			return -1;
		}

		dcb.BaudRate = BaudRate; //9600;
		dcb.ByteSize = ByteSize; //8
		dcb.Parity = 0; //奇偶校验
		dcb.StopBits = 1; //停止数位

		if (!SetCommState(hCom, &dcb))
		{
			DWORD dwError = GetLastError();
			return -1;
		}

		if (!PurgeComm(hCom, PURGE_RXCLEAR)) return -1;

		SetupComm(hCom, 1024, 1024);
	}
	return 0;
}

int sys_log_files(char* filename, char* szFormat, ...)
{
	struct tm* Time;
	time_t				Timet;
	struct _timeb		Timeb;
	static char			buff[110][10240];
	static int			pos;
	char				TimeStr[10240];	/* 保存时间 */
	char				timestamp[10240];  /* 保存时间格式 */
	char                filename1[256];
	static char         oldfilename[256];
	FILE* fp;
	static  int count;
	int i;
	char szBuffer[60480];
	va_list pArguments;

	va_start(pArguments, szFormat);
	vsprintf(szBuffer, szFormat, pArguments);

	timestamp[0] = '\0';

	TimeStr[0] = '\0';
	time(&Timet);
	Time = localtime(&Timet);
	_ftime(&Timeb);

	sprintf(TimeStr, "%02d/%02d %02d:%02d:%02d.%03d ", Time->tm_mon + 1,
		Time->tm_mday, Time->tm_hour, Time->tm_min, Time->tm_sec,
		Timeb.millitm);
	strcat(timestamp, TimeStr);

	if (count % 10000 == 0)
	{
		sprintf(filename1, "%s%02d%02d%02d%02d%02d.log", filename,
			Time->tm_mon + 1, Time->tm_mday, Time->tm_hour, Time->tm_min, Time->tm_sec);
		strcpy(oldfilename, filename1);
	}
	else
		sprintf(filename1, "%s", oldfilename);

	count++;

	pos++;
	if (pos >= logo_cache)
	{
		fp = fopen(filename1, "a");
		if (fp == NULL) { pos = 0; return 0; }
		for (i = 1; i < pos; i++)
		{
			fprintf(fp, "%s", buff[i]);
		}
		fprintf(fp, "%s %s\n", timestamp, szBuffer);
		fclose(fp);
		memset(buff, 0, sizeof(buff));
		pos = 0;
	}
	else
	{
		sprintf(buff[pos], "%s %s\n", timestamp, szBuffer);
	}
	return 0;
}

int SendOutNetwork(char* buff, int size)
{
	char tmp[1024];
	int alls = 0, i;
	short* s = (short*)buff;
	short* s1 = s;

	for (i = 0; i < size / 2; i++)
	{
		alls = alls + abs(s[i]);
	}

	if (alls > min_voc_eng * size / 2) ///speek size=800,alls=40970
	{
		unrecorded_times = 0;
		nospeak = 0;
		recorded = true;
		AddSamples(voice, s1, size / 2);
		if (!(doWakeUp && (!WakeUp)))
		{
			printf("speak size=%d,alls=%d\n", size, alls);
		}
		else if (doCom)
		{
			Serial_write(strSendString.c_str(), strSendString.length());
		}
	}
	else if ((recorded == true) && (unrecorded_times < 3))
	{
		AddSamples(voice, s1, size / 2); //pause
		unrecorded_times++;
		nospeak++;
	}
	else
	{
		unrecorded_times++;
		nospeak++;

		if (nospeak >= NoSpeak_Time)
		{
			if ((doWakeUp) && (nospeak == NoSpeak_Time) && (WakeUp == true))
			{
				cout << "Wakeup close!" << endl;
				cout << "Please say wakeup word: " << WakeUp_word.c_str() << endl;
			}
			WakeUp = false;
		}
	}

	if (voice->len > MAX_UTT_LEN * 0.8) unrecorded_times += 5;  //to avoid too long utterance

	if ((recorded == true) && (unrecorded_times >= 4))
	{
		//WaveSaveStero("test_stero.wav",voice->buf,voice->len);
		WaveSave("test.wav", voice->buf, voice->len);
		//printf("Save test.wav\n");
		recognized_times++;

		P();
		if (buffer_ok == 0)
		{
			if (pWavBuffer != NULL)
			{
				delete[] pWavBuffer; pWavBuffer = NULL;
			}
			pWavBuffer = (short*)malloc(voice->len * sizeof(short));
			memcpy(pWavBuffer, voice->buf, voice->len * sizeof(short));
			pWavBufferLen = voice->len;
			buffer_ok = 1;
		}
		V();

		ClearSamples(voice);
		delete[] voice; voice = NULL;

		voice = new VOICE_HANDLE;

		unrecorded_times = 0;
		recorded = false;
	}
	return 0;
}

int main(int agrc, char* agrv[])
{
	int counter_threads;
	char tmp[103];

	//初始化
	if (ASR_SUCCEEDED_OK == TSASR_Init("./tsASR.ini", 1))
	{
		cout << "tsASR init success!" << endl;
	}
	else
	{
		cout << "tsASR init fails!" << endl;
		return -1;
	}

	InitMicPlayer();

	if (doWakeUp)
	{
		cout << "Please say wakeup word: " << WakeUp_word.c_str() << endl;
	}

	text = (char*)malloc(sizeof(char) * MAX_LEN);
	voice = new VOICE_HANDLE;

	Init_Lock();

	CreateThread(NULL, 0, thread_ASR, NULL, 0, NULL);

	struct _timeb Timeb;

	char ch;
	int c = 0;
	while (1)
	{
		if (kbhit())
		{
			ch = getch();
			if ('Q' == ch) { break; }
		}
		Sleep(1000);
	}

	return 0;
}

DWORD WINAPI thread_ASR(LPVOID lpParamter)
{
	int eos = 0;
	short* pWavBuffer2;
	unsigned long pWavBufferLen2;

	//printf("thread_ASR started\n");
	while (1)
	{
		Sleep(100);

		P();
		if (buffer_ok == 1)
		{
			eos = 1;

			pWavBuffer2 = (short*)malloc(pWavBufferLen * sizeof(short));
			memcpy(pWavBuffer2, pWavBuffer, pWavBufferLen * sizeof(short));
			pWavBufferLen2 = pWavBufferLen;
			buffer_ok = 0;
		}

		V();

		if (eos == 1)
		{
			//printf("thread_ASR asr...\n");

			if (pWavBufferLen2 > MAX_UTT_LEN) //10s
			{
				cout << "Too long utterances!" << endl;

				delete[] pWavBuffer2; pWavBuffer2 = NULL;
				eos = 0;

				continue;
			}

			string token;
			TSASR_Open(tsASR);
			//if(ASR_SUCCEEDED_OK == ASR_recSpeechBuf(hASR,pWavBuffer2,pWavBufferLen2,text,scoreASR))
			if (ASR_SUCCEEDED_OK == TSASR_recSpeechSI(tsASR, pWavBuffer2, pWavBufferLen2, text, numResult, scoreASR))
			{
				string str = text;
				trim_filler(str);
				int tag = str.find(WakeUp_word.c_str());
				if (tag >= 0)
				{
					token = str.substr(tag, WakeUp_word.length());
					trim(token);
					if (!token.compare(WakeUp_word.c_str()))
					{
						WakeUp = true;
						recognized_times = 0;
						nospeak = 0;
					}
				}

				if (!(doWakeUp && (!WakeUp)))
				{
					//cout<<"scoreASR: "<<scoreASR<<endl;
					printf("%.1f\n", scoreASR);
					if (str.length() > 0)
						cout << "text: " << str.c_str() << endl;
					else
						cout << "Rejected!" << endl;
					if(str == "xia men da xue")
						operate(str);
					if (strcmp(str.c_str(), "da kai teng xun") == 0)
						ShellExecute(NULL, "open", "C:\\Program Files (x86)\\Tencent\\QQ\\Bin\\QQ.exe", NULL, NULL, SW_SHOWNORMAL);
					if (strcmp(str.c_str(), "da kai ji shi ben") == 0)
						ShellExecute(NULL, "open", "C:\\Windows\\System32\\notepad.exe", NULL, NULL, SW_SHOWNORMAL);
					if (strcmp(str.c_str(), "da kai hui bian") == 0)
						ShellExecute(NULL, "open", "C:\\emu8086\\emu8086.exe", NULL, NULL, SW_SHOWNORMAL);
					if (strcmp(str.c_str(), "da kai ren wu guan li qi") == 0)
						ShellExecute(NULL, "open", "C:\\Windows\\System32\\Taskmgr.exe", NULL, NULL, SW_SHOWNORMAL);
					if (str.length() > 0)
						operate2(str);
					//if((token.compare(WakeUp_word.c_str()))&&(WakeUp))
					//{
					//	WakeUp = false;
					//	cout<<"Wakeup close!"<<endl;
					//	cout<<"Please say wakeup word: "<<WakeUp_word.c_str()<<endl;
					//}
				}
			}
			else
			{
				cout << "Recognize error!" << endl;
			}

			TSASR_Close(tsASR);

			delete[] pWavBuffer2; pWavBuffer2 = NULL;

			eos = 0;
		}
	}

	return 0;
}