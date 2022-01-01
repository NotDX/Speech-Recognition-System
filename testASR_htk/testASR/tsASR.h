#ifndef tsASR_H
#define tsASR_H

#ifndef tsASR_DLL
#define tsASR_DLL __declspec(dllexport)
#else
#define tsASR_DLL __declspec(dllimport)
#endif

#ifdef	__cplusplus
extern "C"	{
#endif

typedef int Handle;

#include <float.h>

#define NULL 0

enum return_ASR_Code
{
	ASR_SUCCEEDED_OK=0,			//	�����ɹ� 
	ASR_WORKINGDIR_NOT_FIND,	//	����Ŀ¼������
	ASR_STATE_ERROR,			//	���״̬����
	ASR_HANDLE_ERROR,			//	�����ʶ����
	ASR_LICENSE_ERROR,			//	��Ȩ����
	ASR_CONFIG_FILE_NOT_FOUND,	//	�����ļ�δ�ҵ�
	ASR_TRAIN_FILE_NOT_FOUND,	//	ѵ���ļ�δ�ҵ�
	ASR_TRAIN_FILE_NOT_MATCH,	//	ѵ���ļ���ƥ��
	ASR_TRAIN_PATH_NOT_FOUND,	//	ѵ��Ŀ¼δ�ҵ�
	ASR_MODEL_FILE_NOT_FOUND,   //  ģ���ļ�δ�ҵ�
	ASR_TOO_SHORT_SPEECH,       //  �����ļ�̫��
	TSASR_BUSY,					//	��·��æ
	TSASR_IDLE,					//	��·����
	TSASR_CLOSE,				//	��·�ر�
	ASR_OTHER_ERROR				//	��������
};

/************************************************************************/
/* 
						��ʼ������
		@ working_dir	: ����Ŀ¼�������湤����������ļ�
		@ max_lines		: ����֧���������
		@ return		: �ɹ���ʼ������SUCCESS
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_Init(const char* config_file,int max_lines);

/************************************************************************/
/* 
				�ر����棺�ı����·״̬ΪCLOSE
                                                                     */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_Release();

/************************************************************************/
/* 
				����·��h����·��Դ��ʶ
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_Open(Handle &outh);

/************************************************************************/
/* 
				�ر���·��h����·��Դ��ʶ
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_Close(Handle &h);

/************************************************************************/
/* 
					   ѵ������ģ��hmm(���ڻ�����)
        @ handle	     : ��·��Դ��ʶ���������Ѵ򿪣�
		@ count          : ѵ��������������������
		@ buffer         : ѵ����������������
		@ length         : ���ÿ������������
		@ text           : ���ÿ����������Ӧ�ı�����
		@ hmm_model_file : ģ���ļ���������·����
		@ return		 : �ɹ���ʼ������SUCCESS
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_trainSpeechBuf(Handle handle,unsigned int count, short* buffer[],unsigned long length[],char* text[],const char* hmm_model_file);

/************************************************************************/
/* 
					   ��������������������ʶ��
		@ handle		: ��·��Դ��ʶ���������Ѵ򿪣�
		@ buffer        : ��ʶ������������
		@ length        : ��ʶ����������������
		@ text          : ��ʶ����ı�����
		@ hmm_model_file: ��ʶ���HMMģ��
		@ numWord       : ʶ�𵥴ʸ���
		@ scoreASR      : ����ʶ��÷�
		@ return		: �ɹ�ʶ�𷵻�SUCCESS
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_recSpeechBuf(Handle handle,short* buffer,unsigned long length,const char* text,const char* hmm_model_file,int &numWord,float &scoreASR);

/************************************************************************/
/* 
					   ��������������������ʶ�𣨷��ض��ˣ�
		@ handle		: ��·��Դ��ʶ���������Ѵ򿪣�
		@ buffer        : ��ʶ������������
		@ length        : ��ʶ����������������
		@ text          : ʶ����ı�����
		@ numWord       : ʶ�𵥴ʸ���
		@ scoreASR      : ����ʶ��÷�
		@ return		: �ɹ�ʶ�𷵻�SUCCESS
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_recSpeechSI(Handle handle,short* buffer,unsigned long length,char* text,int &numWord,float &scoreASR);

/************************************************************************/
/* 
					   �������÷�
		@ scoreASR      : ����ʶ��÷�
		@ scoreVPR      : ����ʶ��÷�
		@ return		: �������÷�
                                                                        */
/************************************************************************/
tsASR_DLL float TSASR_GetFinalScore(const float scoreASR,const float scoreVPR);

#ifdef __cplusplus
}
#endif

#endif