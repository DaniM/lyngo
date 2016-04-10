#pragma once

#include "LyngoUids.h"

#include "audio/VstProcessor.h"

#include "external/kiss_fft130/kiss_fft.h"
#include "external/kiss_fft130/tools/kiss_fftr.h"

#include "utils/MathUtils.h"

#include <array>
#include <complex>

// Dsp processor which performs fft fir filtering
class FftFir
	: public VstFixedBlockSizeProcessor
{

public:

	FftFir(std::array<std::complex<float>, IrFreqDomainSize>& irFreq
		   , float&											  irTimeAccum);
	~FftFir();

private:

	void ProcessBlock(const float* const in
					  , float* const     out
					  , unsigned		 sizeInSamples) override;

	const float GainAdjustment = 1.0f / static_cast<float>(ZPadBlockSize);

	// FFT
	kiss_fftr_cfg		 forward;
	kiss_fftr_cfg		 inverse;
	std::complex<float>* fwdOut;
	float*				 invOut;

	float*				 shiftHelper;
	float*				 zPadHelper;
	float*				 olapHelper;

	std::array<std::complex<float>, IrFreqDomainSize>& irFreq;
	float&											   irTimeAccum;

	// Applies circular shift to given buffer
	void Shift(float* data
	 		   , unsigned sizeInSamples);

	// Applies Zero Padding to given buffer
	float* ZeroPad(const float* const data
			       , unsigned sizeInSamples);
};

