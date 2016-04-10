#pragma once

#include <math.h>

#define PI acosf(-1)
#define TWO_PI 2.0f * static_cast<float>(PI)

#include "external\kiss_fft130\kiss_fft.h"

// Structure to hold fft bis expressed in polar format
struct FftPolarBin
{
	float magnitude;
	float phase;
};

// Converts an Fft bin from rectangular to polar format
void RectangularToPolar(kiss_fft_cpx* inRectangular, FftPolarBin* outPolar);

// Converts an Fft bin from polar to rectangular format
void PolarToRectangular(kiss_fft_cpx* outRectangular, FftPolarBin* inPolar);

// Performs complex multiplication
kiss_fft_cpx ComplexMultiplication(kiss_fft_cpx a, kiss_fft_cpx b);

// Returns the principal phase argument so that princarg(2*PI*n + phi) = phi, where -PI < phi <= PI.
float PrincArg(float phase);

// Performs modulo operation
float ModF(float x, float m);

// Rounds up value to nearest multiple of factor
unsigned RoundUpToNearestMultiple(unsigned value, unsigned factor);

// Rounds up value to next power of two
unsigned RoundUpToNextPowerOfTwo(unsigned value);

