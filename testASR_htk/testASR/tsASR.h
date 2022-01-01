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
	ASR_SUCCEEDED_OK=0,			//	操作成功 
	ASR_WORKINGDIR_NOT_FIND,	//	工作目录不存在
	ASR_STATE_ERROR,			//	句柄状态有误
	ASR_HANDLE_ERROR,			//	句柄标识有误
	ASR_LICENSE_ERROR,			//	授权有误
	ASR_CONFIG_FILE_NOT_FOUND,	//	配置文件未找到
	ASR_TRAIN_FILE_NOT_FOUND,	//	训练文件未找到
	ASR_TRAIN_FILE_NOT_MATCH,	//	训练文件不匹配
	ASR_TRAIN_PATH_NOT_FOUND,	//	训练目录未找到
	ASR_MODEL_FILE_NOT_FOUND,   //  模型文件未找到
	ASR_TOO_SHORT_SPEECH,       //  语音文件太短
	TSASR_BUSY,					//	线路正忙
	TSASR_IDLE,					//	线路空闲
	TSASR_CLOSE,				//	线路关闭
	ASR_OTHER_ERROR				//	其他错误
};

/************************************************************************/
/* 
						初始化引擎
		@ working_dir	: 工作目录下有引擎工作所必需的文件
		@ max_lines		: 引擎支持最大线数
		@ return		: 成功初始化返回SUCCESS
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_Init(const char* config_file,int max_lines);

/************************************************************************/
/* 
				关闭引擎：改变各线路状态为CLOSE
                                                                     */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_Release();

/************************************************************************/
/* 
				打开线路：h是线路资源标识
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_Open(Handle &outh);

/************************************************************************/
/* 
				关闭线路：h是线路资源标识
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_Close(Handle &h);

/************************************************************************/
/* 
					   训练语音模型hmm(基于缓冲区)
        @ handle	     : 线路资源标识（必须是已打开）
		@ count          : 训练语音（缓冲区）个数
		@ buffer         : 训练语音（缓冲区）
		@ length         : 存放每个缓冲区长度
		@ text           : 存放每个缓冲区对应文本内容
		@ hmm_model_file : 模型文件名（完整路径）
		@ return		 : 成功初始化返回SUCCESS
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_trainSpeechBuf(Handle handle,unsigned int count, short* buffer[],unsigned long length[],char* text[],const char* hmm_model_file);

/************************************************************************/
/* 
					   基于语音缓冲区的语音识别
		@ handle		: 线路资源标识（必须是已打开）
		@ buffer        : 待识别语音缓冲区
		@ length        : 待识别语音缓冲区长度
		@ text          : 待识别的文本内容
		@ hmm_model_file: 待识别的HMM模型
		@ numWord       : 识别单词个数
		@ scoreASR      : 语音识别得分
		@ return		: 成功识别返回SUCCESS
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_recSpeechBuf(Handle handle,short* buffer,unsigned long length,const char* text,const char* hmm_model_file,int &numWord,float &scoreASR);

/************************************************************************/
/* 
					   基于语音缓冲区的语音识别（非特定人）
		@ handle		: 线路资源标识（必须是已打开）
		@ buffer        : 待识别语音缓冲区
		@ length        : 待识别语音缓冲区长度
		@ text          : 识别的文本内容
		@ numWord       : 识别单词个数
		@ scoreASR      : 语音识别得分
		@ return		: 成功识别返回SUCCESS
                                                                        */
/************************************************************************/
tsASR_DLL return_ASR_Code TSASR_recSpeechSI(Handle handle,short* buffer,unsigned long length,char* text,int &numWord,float &scoreASR);

/************************************************************************/
/* 
					   计算最后得分
		@ scoreASR      : 语音识别得分
		@ scoreVPR      : 声纹识别得分
		@ return		: 返回最后得分
                                                                        */
/************************************************************************/
tsASR_DLL float TSASR_GetFinalScore(const float scoreASR,const float scoreVPR);

#ifdef __cplusplus
}
#endif

#endif