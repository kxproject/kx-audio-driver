#ifndef TESTS_H
#define TESTS_H

/*********************** ALL THE EXPORTED TEST FUNCTIONS ARE DECLARED HERE *********************/

#include "Common.h"		// Declarations of some common types and structures

// Functions which generate output waveforms for the tests.
// Input:  "XL" and "XR" are allocated at host application to the float["Size"],
//         "SamplingRate" and "Resolution" are the desired parameters of output waveform
//                                         ("Resolution" is not needed since the waveform is float,
//         optional pointers to temporary data arrays (to prevent allocating memory at DLL).
// Output: "XL" and "XR" must be filled with generated waveform
void FRGenerateOutput(float *XL, float *XR, int Size, float SamplingRate, int Resolution);
void NsGenerateOutput(float *XL, float *XR, int Size, float SamplingRate, int Resolution);
void DRGenerateOutput(float *XL, float *XR, int Size, float SamplingRate, int Resolution);
void THDGenerateOutput(float *XL, float *XR, int Size, float SamplingRate, int Resolution);
void CTGenerateOutput(float *XL, float *XR, int Size, float SamplingRate, int Resolution, float *XL2, float *XR2, int Size2, float SamplingRate2, int Resolution2);

// Functions which analyze the input (recorded) waveforms and return results for the tests.
// Input:  recorded waveform (see previos functions),
//         pointer to test result class which was allocated at host application (including spectrum data class),
//         optional pointers to temporary data arrays (to prevent allocating memory at DLL).
// Output: Result class is filled with data to return to the host application.
void FRAnalyzeInput(float *XL, float *XR, int Size, float SamplingRate, int Resolution, FRResult *D, Cmplx *Y, float *A);
void NsAnalyzeInput(float *XL, float *XR, int Size, float SamplingRate, int Resolution, NsResult *D, Cmplx *Y, float *A, Cmplx *Data, Cmplx *ACurve);
void DRAnalyzeInput(float *XL, float *XR, int Size, float SamplingRate, int Resolution, DRResult *D, Cmplx *X, float *A, Cmplx *Data, Cmplx *ACurve);
void THDAnalyzeInput(float *XL, float *XR, int Size, float SamplingRate, int Resolution, THDResult *D, Cmplx *Y, float *A, Cmplx *Data, float *Amps, Cmplx *ACurve);
void CTAnalyzeInput(float *XL, float *XR, int Size, float SamplingRate, int Resolution, CTResult *D, Cmplx *Y, float *A);

#endif