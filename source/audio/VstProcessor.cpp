#include "audio/VstProcessor.h"

#include <algorithm>
#include <cassert>
#include <stdlib.h>

VstProcessor::~VstProcessor()
{
}

VstFixedBlockSizeProcessor::VstFixedBlockSizeProcessor(unsigned processBlockSize)
	: ProcessBlockSize(processBlockSize)
	, dataIn(sizeof(float)
			 , 2 * ProcessBlockSize)
	, dataOut(sizeof(float)
			  , 2 * ProcessBlockSize)
	, processHelper(nullptr)
{
	processHelper = reinterpret_cast<float*>(malloc(ProcessBlockSize * sizeof(float)));
	assert(processHelper);
}

VstFixedBlockSizeProcessor::~VstFixedBlockSizeProcessor()
{
	free(processHelper);
	processHelper = nullptr;
}

void VstFixedBlockSizeProcessor::Process(const float * const in
										 , float * const     out
									     , unsigned          sizeInSamples)
{
	unsigned samplesLeft  = sizeInSamples;
	const float* processIn  = in;
	float*       processOut = out;
	while (samplesLeft > 0)
	{
		const int processSamples = std::min(samplesLeft, ProcessBlockSize);

		dataIn.Write(processIn, processSamples);

		if (dataIn.GetAvailableElementsForReading() >= ProcessBlockSize)
		{
			dataIn.Read  (processHelper, ProcessBlockSize);
			ProcessBlock (processHelper, processHelper, ProcessBlockSize);
			dataOut.Write(processHelper, ProcessBlockSize);
		}

		if (dataOut.GetAvailableElementsForReading() >= processSamples)
			dataOut.Read(processOut, processSamples);

		processIn   += processSamples;
		processOut  += processSamples;
		samplesLeft -= processSamples;
	}
}
