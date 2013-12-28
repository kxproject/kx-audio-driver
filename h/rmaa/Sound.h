/**************** WAVEFORM ROUTINES MODULE ******************/

#ifndef sound_h
#define sound_h

#define CHANNEL_LEFT	0
#define CHANNEL_RIGHT	1
#define CHANNEL_BOTH	2

#include <stdlib.h>
#include <math.h>
#include "db.h"

#define sqr(x) ((x)*(x))

// Caution: any waveform is STEREO
// AUTODELETE = FALSE
class TWaveformData {
public:
	TWaveformData(float *XL, float *XR, int Size, float SamplingRate, int Resolution);
	float	*XL;
	float	*XR;
	int	Size;
	float	SamplingRate;		// Sampling rate (Hz)
	int	Resolution;		// Bits per sample

	void	SetChannel(int C);
	void	Silence(int Start, int Len);
	void	AddSineWave(int Start, int Len, float Freq, float AmpDB);
	float	GetDC(int EmptyStart, int EmptyLen);
	void	RemoveDC(int EmptyStart, int EmptyLen);
	float	GetRMS(int Start, int Len);
	float	GetPeak(int Start, int Len);
	int	FindSignal(int Start, int Len, int LevelDB, int BlockSize);
	int	FindPause(int Start, int Len, int LevelDB, int BlockSize);
	void	ApplyDither(int Resolution);
	// Waveform time metrics conversions
	float	T2S(float T);		// Time (sec) to samples
	float	S2T(float S);		// Samples to time (sec)
	float	H2S(float Hz);		// Hz to samples
	float	S2H(float S);		// Samples to Hz
private:
	void	CommonInit();
	DB	db;
	float	*CurX;
	int	Channel;
};

#endif

