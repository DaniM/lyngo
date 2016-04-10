#include "MathUtils.h"

//  --------------------------------------------------------------------------------
/*
	Converts an Fft bin from rectangular to polar format
*/

void RectangularToPolar(kiss_fft_cpx* inRectangular
						, FftPolarBin* outPolar)
{
	outPolar->magnitude = (sqrtf(inRectangular->r * inRectangular->r + inRectangular->i * inRectangular->i));
	outPolar->phase = (atan2f(inRectangular->i ,  inRectangular->r ));
}


//  --------------------------------------------------------------------------------
/*
	 Converts an Fft bin from polar to rectangular format
*/

void PolarToRectangular(kiss_fft_cpx* outRectangular
						, FftPolarBin* inPolar)
{
	outRectangular->r = (inPolar->magnitude * cosf(inPolar->phase));
	outRectangular->i = (inPolar->magnitude * sinf(inPolar->phase));
}

kiss_fft_cpx ComplexMultiplication(kiss_fft_cpx a
								   , kiss_fft_cpx b)
{
	kiss_fft_cpx ret;
	ret.r = a.r * b.r - a.i * b.i;
	ret.i = a.r * b.i + a.i * b.r;
	return ret;
}


//  --------------------------------------------------------------------------------
/*
	Returns the principal phase argument so that princarg(2*PI*n + phi) = phi, where -PI < phi <= PI.
*/
float PrincArg
	(
		float phase
	)
{
	return ModF(phase + PI,TWO_PI) - PI;
}

//  --------------------------------------------------------------------------------
/*
	 Performs modulo operation
*/
float ModF
	(
		float x,
		float m
	)
{
	return x - (m * floorf(x / m));
}

//  --------------------------------------------------------------------------------
/*
	Rounds up value to nearest multiple of factor
*/
unsigned RoundUpToNearestMultiple
	(
		unsigned value,
		unsigned factor
	)
{
	unsigned retValue = value;
	unsigned const remainder = value % factor;
	if (remainder != 0u)
	{
		retValue += (factor - remainder);
	}
	return retValue;
}

//  --------------------------------------------------------------------------------
/*
	Rounds up value to next power of two
*/
unsigned RoundUpToNextPowerOfTwo
(
	unsigned value
)
{
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value++;

	return value;
}
