
#include <float.h>
#include "../ufw/ufw.h"
#include "umath.h"
#include "fresp_tbl.h"

#pragma warning(disable: 4514) 

//................................................................................

const double LOG2 = 0.693147180559945; // log(2)

#define	a biquad->a
#define	b biquad->b

void reset_biquad(biquad_t* biquad)
{
	b[0] = 1.; b[1] = 0.; b[2] = 0.; 
	a[0] = 1.; a[1] = 0.; a[2] = 0.;
}

int compute_biquad(biquad_t* biquad, int type, float f, float g, float q) 
{
	double w, K, S, t, tt;

	#define NOZEROG if (g == 0.f) {reset_biquad(biquad); return 0;}
	#define HILIMIT 23808.f
	if (f > HILIMIT) f = HILIMIT;

	trace("biquad: f %.f, g %.01f, q %.02f\n", f, g, q);

	w = 2. * pi * f/FS;
	t = tan(w * .5);
	tt = t * t;
	K = dBtoG(g);

	switch (type)
	{
	case BAND_PASS: {
		S = 2. * sinh(.5 * LOG2 * q);
		a[0] = 1. + t*S + tt;
		a[1] = 2. * (tt - 1.);
		a[2] = 1. - t*S + tt;
		b[0] = t*S;
		b[1] = 0.;
		b[2] = -t*S;
		break; }

	case NOTCH: {
		S = 2. * sinh(.5 * LOG2 * q);
		a[0] = 1. + t*S + tt;
		a[1] = 2. * (tt - 1.);
		a[2] = 1. - t*S + tt;
		b[0] = 1. + tt;
		b[1] = 2. * (tt - 1.);
		b[2] = 1. + tt;
		break; }

	case LO_PASS: {
		S = sqrt(2.)/q;
		a[0] = 1. + t*S + tt;
		a[1] = 2. * (tt - 1.);
		a[2] = 1. - t*S + tt;
		b[0] = tt;
		b[1] = 2. * (tt);
		b[2] = tt;
		break; }

	case HI_PASS: {
		S = sqrt(2.)/q;
		a[0] = 1. + t*S + tt;
		a[1] = 2. * (tt - 1.);
		a[2] = 1. - t*S + tt;
		b[0] = 1.;
		b[1] = -2.;
		b[2] = 1.;
		break; }

	case LO_S_SHELF: {
		NOZEROG;
		double IK, rK, rIK; // fixme, do these explicit subexpressions make sense at all?
		S = sqrt(2.)/q;
		K = sqrt(K);
		IK = 1./K;
		rK = sqrt(K);
		rIK = sqrt(IK);
		a[0] = 1. + rIK*t*S + tt*IK;
		a[1] = 2. * (tt*IK - 1.);
		a[2] = 1. - rIK*t*S + tt*IK;
		b[0] = 1. + rK*t*S + tt*K;
		b[1] = 2. * (tt*K - 1.);
		b[2] = 1. - rK*t*S + tt*K;
		break; }

	case LO_R_SHELF: {
		NOZEROG;
		double A, B, C, D, X, Y;
		K = sqrt(K);
		S = q * sqrt(2.);
		X = sqrt(2. / (S - 1.));
		Y = sqrt((S + 1.) / (S - 1.));

		A = X;
		B = Y / K;
		C = X;
		D = Y * K;

		a[0] = 1. + t*A + tt*B;
		a[1] = 2. * (tt*B - 1.);
		a[2] = 1. - t*A + tt*B;
		b[0] = 1. + t*C + tt*D;
		b[1] = 2. * (tt*D - 1.);
		b[2] = 1. - t*C + tt*D;
		break; }
	
	case HI_S_SHELF: {
		NOZEROG;
		S = sqrt(2.)/q;
		K = sqrt(K);
		a[0] = 1./K + sqrt(1./K)*t*S + tt;
		a[1] = 2. * (tt - 1./K);
		a[2] = 1./K - sqrt(1./K)*t*S + tt;
		b[0] = K + sqrt(K)*t*S + tt;
		b[1] = 2. * (tt - K);
		b[2] = K - sqrt(K)*t*S + tt;
		break; }

	case HI_R_SHELF: {
		NOZEROG;
		double A, B, C, D, X, Y;
		t = tan((pi - w)*.5);
		tt = t*t;
		K = sqrt(K);
		S = q * sqrt(2.);
		X = sqrt(2. / (S - 1.));
		Y = sqrt((S + 1.) / (S - 1.));

		A = X;
		B = Y / K;
		C = X;
		D = Y * K;

		a[0] = 1. + t*A + tt*B;
		a[1] = -2. * (tt*B - 1.);
		a[2] = 1. - t*A + tt*B;
		b[0] = 1. + t*C + tt*D;
		b[1] = -2. * (tt*D - 1.);
		b[2] = 1. - t*C + tt*D;
		break; }

	case BELL2: {
		NOZEROG;
		// see 'ftcwbl_opt.m' for reference
		double BW, A, B, C, D, M, W2, dW, Om1, Om2;
		double K00, L00, KN, K01, K11, L01, wX, num, den;
		BW = q;
		S = 2. * w * sinh(BW * LOG2 * .5);
		M   = fabs(K*K - K);
		K00 = fabs(K*K - 1.);
		L00 = fabs(K - 1.);

		double piSsqr, wsubpi;
		piSsqr = pi*pi * S*S;
		wsubpi = w*w - pi*pi; wsubpi = wsubpi*wsubpi;
		num = wsubpi + K * piSsqr;
		den = wsubpi + (1./K) * piSsqr;
		KN = sqrt(num/den);

		K01 = fabs(K*K - KN);
		K11 = fabs(K*K - KN*KN);
		L01 = fabs(K - KN);

		W2 = sqrt(K11 / K00) * tt; 

		wX = sqrt((K + KN) / (K + 1.)) * tt;
		Om1 = tan(w * exp(-.5 * LOG2 * (2. + BW))); // tan(w * 2^(-BW*.5-1.))
		Om2 = wX / Om1;
		dW = (Om2 - Om1);
    
		C = dW*dW * L01 - 2. * W2 * (L01 - sqrt(L00 * L01));
		D = 2. * W2 * (K01 - sqrt(K11 * K00));
		A = sqrt((C + D) / M);
		B = sqrt(K * (K * C + D) / M);

		a[0] = 1. + A + W2; 										
		a[1] = 2. * (W2 - 1.); 				
		a[2] = 1. - A + W2;
		b[0] = KN + B + W2;
		b[1] = 2. * (W2 - KN);
		b[2] = KN - B + W2;	
		break; }

	default:
		reset_biquad(biquad);
		return 0;
	}

	// normalize
	double norm = 1./a[0];
	b[0] *= norm;
	b[1] *= norm;
	b[2] *= norm;
	a[0] = 1.;
	a[1] *= norm;
	a[2] *= norm;

	trace("biquad: %.08f %.08f %.08f %.08f %.08f\n", b[0], b[1], b[2], a[1], a[2]);
	
	return 0;
}

//................................................................................

const double ILOG2 = 1.44269504088896; // 1/log(2)
const double LOG_FS = log((double) FS);
const int ppo = fresp_points_per_octave;
const int np = fresp_points;

int compute_fresp_log(float mag[], biquad_t* biquad) 
{
	// biquad should be normalized (e.g. a[0] = 1.)
	const double scale = 1000.; // see fresp_tbl.h
	double h, c, c2;
	double bb1 = scale * (b[0]*b[0] + b[1]*b[1] + b[2]*b[2]);
	double bb2 = 2. * (b[0]*b[1] + b[1]*b[2]);
	double bb3 = 2. * (b[0]*b[2]);
	double aa1 = scale * (/* a[0]*a[0]* */ 1. + a[1]*a[1] + a[2]*a[2]);
	double aa2 = 2. * (/* a[0]* */ a[1] + a[1]*a[2]);
	double aa3 = 2. * (/* a[0]* */ a[2]);
	for (int i = 0; i < fresp_points; i++)
	{
		c = fresp_tbl[i];			// scale * cos(2*pi*F/FS)
		c2 = fresp_tbl[i + ppo];	// scale * cos(4*pi*F/FS)
		h = (bb1 + bb2*c + bb3*c2) / (aa1 + aa2*c + aa3*c2);
		mag[i] = (float) (log(fabs(h) + DBL_EPSILON) * 4.342944819029); // m[i] = 20*log10(sqrt(h+eps))
	}
	
	return 0;
}

const double magic_ILOG2xPPO = ILOG2 * ppo;

int find_nearest_fresp_point(float f) 
{
	int i = (int) ((LOG_FS - log(f)) * magic_ILOG2xPPO + .5);
	i = (ppo + np - 1) - i;
	return i;
}

#undef	a
#undef	b

//................................................................................
