/****************** INTERFACE DECLARATIONS ****************/

#ifndef COMMON_H
#define COMMON_H

/***** INCLUDED HEADERS *****/
#include "spdata.h"		// Spectrum data class (for returning spectrum data to the host application)
#include "fft.h"		// DSP routines

/******* CLASSES FOR RETURNING DATA INTO HOST APPLICATION ********/
/******* Statistics (to display at "Details" dialog)      ********/
/******* and the spectrum (to display at "Graph" dialog)  ********/

/***** CROSSTALK TEST *****/
class TCTStat {
public:
	float	CT1L, CT1R;
	float	CT2L, CT2R;
	float	CT3L, CT3R;
};

class CTResult {
public:
	TCTStat stat;
	TFRData *fr;
};


/***** FREQUENCY RESPONSE TEST *****/
class TFRStat {
public:
	float	ALow1, AHigh1;
	float	ALow2, AHigh2;
};

class FRResult {
public:
	TFRStat stat;
	TFRData *fr;
};


/***** NOISE LEVEL TEST RESULT *****/
class TNoiseStat {
public:
	float	RMSL, RMSR;
	float	RMSAL, RMSAR;
	float	PeakL, PeakR;
	float	DCL, DCR;
};

class NsResult {
public:
	TNoiseStat stat;
	TSpectrumData *sd;
};


/***** DYNAMIC RANGE TEST RESULT *****/
class TDRStat {
public:
	float	DRL, DRR;
	float	DRAL, DRAR;
	float	DCL, DCR;
};

class DRResult {
public:
	TDRStat stat;
	TSpectrumData *sd;
};


/***** THD TEST RESULT *****/
class TTHDStat {
public:
	float	THDL, THDR;
	float	THDNL, THDNR;
	float	THDNAL, THDNAR;
	float	DCL, DCR;
};

class THDResult {
public:
	TTHDStat stat;
	TSpectrumData *sd;
};

struct Harmonics {
	float	Freq;
	float	OutAmp;
	float	InAmp;
};

#define NUM_HARM	136

extern DB	db;
extern FFT	fft;
extern Harmonics X[NUM_HARM];
extern float FindSpectrumPeak(float *A, float CFreq, float MaxDev, float FStep);

#endif
