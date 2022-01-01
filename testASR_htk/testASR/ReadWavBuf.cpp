#include "ReadWavBuf.h"
#include <stdlib.h>
#include <stdio.h>
#include <String.h>
#include <io.h>

int AddSamples(VOICE_HANDLE *&vh, short *pSrc, int len) 
{
	if (vh == NULL) {
		return -1;
	}
	
	// nLen = DelSilence(pSrc, len, pbuf);
	if (len <= 0) return -1;
	
	if (vh->buf == NULL) {
		vh->buf = (short *)malloc(len*sizeof(short));
	} else {
		vh->buf = (short *)realloc(vh->buf, (vh->len+len)*sizeof(short));
	}
	memcpy(vh->buf+vh->len, pSrc, len*sizeof(short));
	vh->len += len;
	
	
	return 0;
}

int ClearSamples(VOICE_HANDLE *&vh)
{
	if (!vh) {
		return -1;
	}
	
	if (vh->buf) {
		delete[] vh->buf; vh->buf = NULL;
		vh->len = 0;
	}
	
	return 0;
}

int ReadWavBuffer(const char *filename, short *&buf, int &len, int skip)
{
	FILE *p = fopen(filename, "rb");
	
	if (!p) {
		return -1;
	}
	
	// get length
	len = (_filelength(_fileno(p))-skip)/2;
	buf = new short[len];
	
	// skip header
	fseek(p, skip, SEEK_CUR);
	
	// read buffer
	fread(buf, sizeof(short), len, p);
	fclose(p);
	
	return 0;
	
}
