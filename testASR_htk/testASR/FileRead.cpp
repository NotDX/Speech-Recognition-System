#include "FileRead.h"
#include <io.h>
#include <stdio.h>

short alaw2linear2( unsigned char a_val )
{
	short t;
	short seg;

	a_val ^= 0x55;

	t = (a_val & QUANT_MASK) << 4;
	seg = ((unsigned short)a_val & SEG_MASK) >> SEG_SHIFT;
	switch (seg) {
	case 0:
		t += 8;
		break;
	case 1:
		t += 0x108;
		break;
	default:
		t += 0x108;
		t <<= seg - 1;
	}
	return ((a_val & SIGN_BIT) ? t : -t);
}

bool ReadVoiceBuffer(const char *filepath, short *&buf, unsigned long &len, int skip, TS_SPEECH_TYPE type /* = TS_WAVE */)
{
	// validity check
	FILE *p = fopen(filepath, "rb");
	if (!p)		return false;

	if (type == TS_WAVE)
	{
		TS_WAV_HEADER fmt;

		fread(&fmt, sizeof(TS_WAV_HEADER), 1, p);

		if (fmt.wChannels != 1 
			|| fmt.wBitsPerSample != 16)
		{
			fclose(p);
			return false;
		}
		
		len = fmt.dwDataSize/2;
		buf = new short[len];
		fread(buf, sizeof(short), len, p);
	}
	else if (type == TS_PCM_RAW)
	{
		len = _filelength(_fileno(p))/2;
		buf = new short[len];
		fread(buf, sizeof(short), len, p);
	}
	else if (type == TS_PCM_ALAW)
	{
		len = _filelength(_fileno(p))-skip;
		buf = new short[len];

		fseek(p, skip, SEEK_CUR);	// Ìø¹ýÍ·²¿

		unsigned char c;		
		for (int i=0;i<len;i++)
		{
			fread(&c, sizeof(char), 1, p);
			buf[i] = alaw2linear2(c);			
		}
	}
	else
	{
		fclose(p);
		return false;
	}

	fclose(p);
	return true;
}

bool WaveSaveStero(const char *wavFilename,short *pBuf,long len)
{
	WAVECHUNK	waveChunk;
	FMTCHUNK	fmtChunk;
	DATACHUNK	dataChunk;
	
	FILE *fp = fopen(wavFilename,"wb");
	if (fp == NULL)
	{
		return false;
	}
	
    waveChunk.riff[0]='R',waveChunk.riff[1]='I',waveChunk.riff[2]='F',waveChunk.riff[3]='F';
    waveChunk.wave[0]='W',waveChunk.wave[1]='A',waveChunk.wave[2]='V',waveChunk.wave[3]='E';
	waveChunk.length = len*sizeof(short)/sizeof(char) + 36;
	fwrite(&waveChunk,sizeof(WAVECHUNK),1,fp);
	
	fmtChunk.chans = 2;
	fmtChunk.bpchan = 16;
	fmtChunk.bpsample = 2;
	fmtChunk.bpsec = 2*SAMPLERATE;	
	fmtChunk.flength = 16;
	fmtChunk.fmt[0]='f',fmtChunk.fmt[1]='m',fmtChunk.fmt[2]='t',fmtChunk.fmt[3]=' ';
	fmtChunk.format = 1;
	fmtChunk.sampsRate = SAMPLERATE;
	fwrite(&fmtChunk,sizeof(FMTCHUNK),1,fp);
	
    dataChunk.data[0]='d',dataChunk.data[1]='a',dataChunk.data[2]='t',dataChunk.data[3]='a';
	dataChunk.dlength = len*sizeof(short)/sizeof(char);
	fwrite(&dataChunk,sizeof(DATACHUNK),1,fp);
	
	fwrite(pBuf,sizeof(short),len,fp);
	
	fclose(fp);
	
	return true;
}

bool WaveSave(const char *wavFilename,short *pBuf,long len)
{
	WAVECHUNK	waveChunk;
	FMTCHUNK	fmtChunk;
	DATACHUNK	dataChunk;
	
	FILE *fp = fopen(wavFilename,"wb");
	if (fp == NULL)
	{
		return false;
	}
	
    waveChunk.riff[0]='R',waveChunk.riff[1]='I',waveChunk.riff[2]='F',waveChunk.riff[3]='F';
    waveChunk.wave[0]='W',waveChunk.wave[1]='A',waveChunk.wave[2]='V',waveChunk.wave[3]='E';
	waveChunk.length = len*sizeof(short)/sizeof(char) + 36;
	fwrite(&waveChunk,sizeof(WAVECHUNK),1,fp);
	
	fmtChunk.chans = 1;
	fmtChunk.bpchan = 16;
	fmtChunk.bpsample = 2;
	fmtChunk.bpsec = 2*SAMPLERATE;	
	fmtChunk.flength = 16;
	fmtChunk.fmt[0]='f',fmtChunk.fmt[1]='m',fmtChunk.fmt[2]='t',fmtChunk.fmt[3]=' ';
	fmtChunk.format = 1;
	fmtChunk.sampsRate = SAMPLERATE;
	fwrite(&fmtChunk,sizeof(FMTCHUNK),1,fp);
	
    dataChunk.data[0]='d',dataChunk.data[1]='a',dataChunk.data[2]='t',dataChunk.data[3]='a';
	dataChunk.dlength = len*sizeof(short)/sizeof(char);
	fwrite(&dataChunk,sizeof(DATACHUNK),1,fp);
	
	fwrite(pBuf,sizeof(short),len,fp);
	
	fclose(fp);
	
	return true;
}