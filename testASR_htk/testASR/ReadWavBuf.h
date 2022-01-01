#ifndef _READ_WAV_BUF_
#define _READ_WAV_BUF_

#include <stdlib.h>
#include <io.h>

struct VOICE_HANDLE {
	short *buf;
	int len;
	
	VOICE_HANDLE() {
		this->buf = NULL;
		this->len = 0;
	}
};


int AddSamples(VOICE_HANDLE *&vh, short *pSrc, int len);

int ClearSamples(VOICE_HANDLE *&vh);

int ReadWavBuffer(const char *filename, short *&buf, int &len, int skip);

#endif	//