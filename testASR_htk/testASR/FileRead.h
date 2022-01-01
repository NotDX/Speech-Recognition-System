#ifndef _TS_WAVEFORM_H_
#define _TS_WAVEFORM_H_

#include <windows.h>

#define SAMPLERATE 16000

typedef enum {
	TS_WAVE = 1,
	TS_PCM_RAW,
	TS_PCM_ALAW
}TS_SPEECH_TYPE;

/// alaw->linear
#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	NSEGS		(8)		/* Number of A-law segments. */
#define	SEG_SHIFT	(4)		/* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */


/// MS_WAV format 
typedef struct {
	char szRiffID[4];  // 'R','I','F','F'
	DWORD dwRiffSize;
	char szRiffFormat[4]; // 'W','A','V','E'

	char  szFmtID[4]; // 'f','m','t',' '
	DWORD  dwFmtSize;
	WORD wFormatTag;
	WORD wChannels;
	DWORD dwSamplesPerSec;
	DWORD dwAvgBytesPerSec;
	WORD wBlockAlign;
	WORD wBitsPerSample;

	char szDataID[4]; // 'd','a','t','a'
	DWORD dwDataSize;
}TS_WAV_HEADER;


typedef struct {
	long	length;		// number of samples in the data chunk
	long	samprate;	// sample rate
	long	bitspsample;	// bits per sample
}WAVINFO;

typedef struct {
	char riff[4];	// RIFF file identification (4 bytes) 
	long length;	// length field (4 bytes)
	char wave[4];	// WAVE chunk identification (4 bytes)
}WAVECHUNK;

typedef struct{
	char fmt[4];	// format sub-chunk identification  (4 bytes)
	long flength;	// length of format sub-chunk (4 byte integer)
	short format;	// format specifier (2 byte integer) 
	short chans;	// number of channels (2 byte integer) 
	long sampsRate;	// sample rate in Hz (4 byte integer) 
	long bpsec;		// bytes per second (4 byte integer) 
	short bpsample;	// bytes per sample (2 byte integer) 
	short bpchan;	// bits per channel (2 byte integer) 
}FMTCHUNK;

typedef struct{
	char szFactID[4];	//'f','a','c','t'
	long dwFactSize;	//the value is 4
}FACTCHUNK;

typedef struct{
	char data[4];	// data sub-chunk identification  (4 bytes) 
	long dlength;	// length of data sub-chunk (4 byte integer)
}DATACHUNK;

/// alaw->linear
/// @param a_val sample of alaw
/// @return short value
short alaw2linear2( unsigned char a_val );	// alaw to linear

/// read data
/// @param p FILE pointer
/// @buf voice buffer
/// @len buffer length
/// @type file format
/// @return true if no expectation
bool ReadVoiceBuffer(const char *filepath, short *&buf, unsigned long &len, int skip, TS_SPEECH_TYPE type = TS_WAVE);
bool WaveSave(const char *wavFilename,short *pBuf,long len);
bool WaveSaveStero(const char *wavFilename,short *pBuf,long len);

#endif	// _TS_WAVEFORM_H_