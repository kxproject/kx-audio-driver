
#ifndef _EQPX_H_
#define _EQPX_H_

//.............................................................................
#include "../ufxkx.h"

//.............................................................................

_inline int is9X()
{
	OSVERSIONINFO os = {sizeof OSVERSIONINFO};
	GetVersionEx(&os);
	return (os.dwMajorVersion < 5) ? 1 : 0;
}

_inline float bw_param_to_q(int type, int v)
{
	double m1[] = {-0.044547, 0.976765, 2.853846};
	double m2[] = {0.677505, 1.545786, 2.650741};
	double K = 5. / 127.;
	switch (type)
	{
	case BELL:
	case BELL2:
	case BAND_PASS:
	case NOTCH:
			if (v > 126) return 4.f;
			else if (v == 1) return 1.f/11.f;
			else if (v == 0) return 1.f/12.f;
			return (float) (m1[0] + pow(2., v * K * m1[1] - m1[2]));
		break;
	case LO_S_SHELF:
	case HI_S_SHELF:
			if (v < 31) return 2.f;
			return (float) pow(2., 1.5 - v * K * .5);
		break;
	case LO_R_SHELF:
	case HI_R_SHELF:
			if (v > 126) return .7072f;
			else if (v < 31) return sqrtf(8.f);
			return (float) (m2[0] + pow(2., m2[2] - v * K * m2[1]));
		break;
	case LO_PASS:
	case HI_PASS:
			return (float) pow(2., 3. - v * K);
		break;
	default:
			return 1.f;
		break;
	}
}

_inline float f_param_to_hz(int v)
{
	double fl = 24, fh = 240;
	return (float) (fl * pow(fh/fl, (1./16384.) * v));
}

_inline int eqp_transform_biquad(const biquad_t* const biquad, double k[], int* L)
{
	k[0] = biquad->b[0];
	k[1] = biquad->b[1] / k[0];
	k[2] = biquad->b[2] / k[0];
	k[4] = -biquad->a[1];
	k[5] = -biquad->a[2];

	int l = 0;

	if ((fabs(k[1]) > 1) | (fabs(k[4]) > 1))
	{
		l = 1;
		k[1] *= 0.5f;
		k[2] *= 0.5f;
		k[4] *= 0.5f;
		k[5] *= 0.5f;
	}

	*L = (int) l;

	return 0;
}

_inline int __sign(int x) // returns -1 for x < 0 and 1 otherwise
{
	return (x < 0) ? -1 : 1;
}

//.............................................................................

#endif // _EQPX_H_