#include "audio/FftFir.h"

#include "external/kiss_fft130/_kiss_fft_guts.h"

#include <algorithm>
#include <cassert>
#include <stdlib.h>
#include <string.h>
#include <math.h>

FftFir::FftFir(std::array<std::complex<float>, IrFreqDomainSize>& irFreq
			   , float&									          irTimeAccum)
	: VstFixedBlockSizeProcessor(BlockSize)
	, forward	 (nullptr)
	, inverse	 (nullptr)
	, fwdOut	 (nullptr)
	, invOut	 (nullptr)
	, shiftHelper(nullptr)
	, zPadHelper (nullptr)
	, olapHelper (nullptr)
	, irFreq	 (irFreq)
	, irTimeAccum(irTimeAccum)
{
	forward = kiss_fftr_alloc(ZPadBlockSize, 0, 0, 0);
	inverse = kiss_fftr_alloc(ZPadBlockSize, 1, 0, 0);

	// Allocate buffer to hold forward output
	fwdOut = new std::complex<float>[IrFreqDomainSize];

	// Allocate buffer to hold inverse output
	const int zPadSizeInBytes = ZPadBlockSize * sizeof(float);
	invOut = reinterpret_cast<float*>(malloc(zPadSizeInBytes));
	memset(invOut, 0, zPadSizeInBytes);

	// Allocate helper buffer for circular shifting operations
	shiftHelper = reinterpret_cast<float*>(malloc(zPadSizeInBytes));
	memset(shiftHelper, 0, zPadSizeInBytes);

	// Allocate helper buffor for zero padding 
	zPadHelper = reinterpret_cast<float*>(malloc(zPadSizeInBytes));
	memset(zPadHelper, 0, zPadSizeInBytes);

	// Allocate overlap add helper
	olapHelper = reinterpret_cast<float*>(malloc(zPadSizeInBytes));
	memset(olapHelper, 0, zPadSizeInBytes);
}

FftFir::~FftFir()
{
	free(forward);
	free(inverse);
	
	delete[] fwdOut;
	fwdOut = nullptr;

	free(invOut);
	invOut = nullptr;

	free(shiftHelper);
	shiftHelper   = nullptr;

	free(zPadHelper);
	zPadHelper    = nullptr;

	free(olapHelper);
	olapHelper    = nullptr;
}

void FftFir::ProcessBlock(const float* const in
					    , float* const  out
					    , unsigned		sizeInSamples)
{
	assert(sizeInSamples == BlockSize);

	const float* zPadInput = ZeroPad(in, BlockSize);

	// do the forward FFT
	kiss_fftr(forward, zPadInput, reinterpret_cast<kiss_fft_cpx*>(fwdOut));

	// convolve with FIR impulse
	for (unsigned i = 0; i < IrFreqDomainSize; ++i)
		fwdOut[i] = fwdOut[i] * irFreq[i];

	// inverse FFT
	kiss_fftri(inverse, reinterpret_cast<kiss_fft_cpx*>(fwdOut), invOut);

	// overlap add and compensate fft output
	for (unsigned sampleIndex = 0; sampleIndex < ZPadBlockSize; ++sampleIndex)
		olapHelper[sampleIndex] += (invOut[sampleIndex] * GainAdjustment / irTimeAccum);

	memcpy(out, olapHelper, BlockSize * sizeof(float));

	// prepare olap helper for the next iteration
	Shift(olapHelper, ZPadBlockSize);
	memset(olapHelper + BlockSize, 0, (ZPadBlockSize - BlockSize) * sizeof(float));	
}

void FftFir::Shift(float* data
				      , unsigned sizeInSamples)
{
	unsigned const halfFrame = sizeInSamples / 2u;
	memcpy(shiftHelper, data, sizeof(float) * sizeInSamples);
	memcpy(data, shiftHelper + halfFrame, sizeof(float) *  halfFrame);
	memcpy(data + halfFrame, shiftHelper, sizeof(float) *  halfFrame);
}

float* FftFir::ZeroPad(const float* const data
	  			       , unsigned sizeInSamples)
{
	assert(sizeInSamples <= ZPadBlockSize);
	memcpy(zPadHelper, data, sizeInSamples * sizeof(float));
	memset(zPadHelper + sizeInSamples, 0, (ZPadBlockSize - sizeInSamples) * sizeof(float));
	return zPadHelper;
}