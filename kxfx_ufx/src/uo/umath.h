#ifndef _U_MATH_H_
#define _U_MATH_H_     

//.............................................................................
#include <math.h>       

//.............................................................................

#define pi	3.14159265358979
#define FS	48000 
#define FN	24000

inline int double2fixed(const double& x) 
{
	double y = (x * 2147483647.5) - 0.25;
	y += 6755399441055744.0;
	return *(int*) &y;
}

inline double dBtoG(const double& d) {return exp(0.1151292546498 * d);}
inline double cBtoG(const double& c) {return exp(.01151292546498 * c);}
inline double GtodB(const double& g) {return log(g) * 8.685889638058;}
inline double GtocB(const double& g) {return log(g) * 86.85889638058;}

//................................................................................

struct biquad_t
{
	double b[3];
	double a[3];
};

int compute_biquad(biquad_t* biquad, int t, float f, float g, float q);
void reset_biquad(biquad_t* biquad);

enum
{
	BELL,
	BELL2,
	BAND_PASS,
	NOTCH,
	LO_S_SHELF,
	HI_S_SHELF,
	LO_R_SHELF,
	HI_R_SHELF,
	LO_PASS,
	HI_PASS,
};

//................................................................................

int compute_fresp_log(float mag[], biquad_t* biquad);
int find_nearest_fresp_point(float f);

#define fresp_points 196 // (sizeof(fresp_tbl) / sizeof(*fresp_tbl))
#define fresp_points_per_octave 19
//................................................................................

#endif // _U_MATH_H_
