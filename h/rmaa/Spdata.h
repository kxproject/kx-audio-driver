#ifndef spdata_h
#define spdata_h

/*************** CLASS FOR STORING A LINEAR FOURIER SPECTRUM *****************/
class TSpectrumData {
public:
	TSpectrumData(int argFFTSize, float argSamplingRate);
	~TSpectrumData();
	float	*XL;		// Amplitudes (dB) array from 0 Hz to (SamplingRate/2) Hz
	float	*XR;		// Amplitudes (dB) array from 0 Hz to (SamplingRate/2) Hz
	float	GetFreqDB(float Freq, int Channel);		// Get the amplitude of a given frequency
	float	SamplingRate;		// Sampling rate (Hz)
	int	FFTSize;		// Size of array = FFTSize/2 + 1
};


// Class for FR bounds
class TFRBounds {
public:
	float	Hi, Lo;
};


/*************** CLASS FOR STORING A FREQUENCY RESPONSE *****************/
class TFRData {
public:
	TFRData(int argSize);
	~TFRData();
	float	*F;		// Frequencies
	float	*XL;		// Response (LEFT)
	float	*XR;		// Response (RIGHT)
	int	Size;		// Size of response (number of points)
	float	SamplingRate;
	float	GetFreqDB(float f);		// Linear interpolation
	TFRBounds	GetBounds(float f1, float f2);		// Gets the accuracy of response
	void	SetChannel(int Ch);
	float	*CurX;
private:
	int	Channel;
};

#endif
