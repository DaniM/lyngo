#pragma once

#include "utils/RingBuffer.h"

// Vst dsp processor
class VstProcessor
{

public:

	~VstProcessor();

	virtual void Process(const float* const in
						 , float* const     out
						 , unsigned			sizeInSamples) = 0;
};

// Vst dsp fixed block size processor
class VstFixedBlockSizeProcessor
	: public VstProcessor
{

public:

	VstFixedBlockSizeProcessor(unsigned processBlockSize);	
	~VstFixedBlockSizeProcessor();

	void Process(const float* const in
				 , float* const     out
				 , unsigned			sizeInSamples) override;

private:

	virtual void ProcessBlock(const float* const in
							  , float* const     out
							  , unsigned		 sizeInSamples) = 0;

	const unsigned ProcessBlockSize;

	RingBuffer dataIn;
	RingBuffer dataOut;
	float*     processHelper;
};

