#pragma once

#include "LyngoUids.h"

#include "external/kiss_fft130/kiss_fft.h"
#include "external/kiss_fft130/tools/kiss_fftr.h"
#include "external/vst3/public.sdk/source/vst/vstaudioeffect.h"
#include "external/vst3/pluginterfaces/vst/ivstparameterchanges.h"

#include <array>
#include <complex>
#include <memory>
#include <mutex>
#include <vector>

class FftFir;
class RingBuffer;

using namespace Steinberg;
using namespace Steinberg::Vst;

class LyngoProcessor
	: public AudioEffect
{

public:

	LyngoProcessor();
	~LyngoProcessor();

	// Steinberg::Vst::AudioEffect
	tresult PLUGIN_API initialize(FUnknown* context)		override;
	tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;
	tresult PLUGIN_API setBusArrangements(SpeakerArrangement*   inputs
										  , int32  			    numIns
										  , SpeakerArrangement* outputs
										  , int32			    numOuts) override;

	tresult PLUGIN_API setActive(TBool state)     override;
	tresult PLUGIN_API process(ProcessData& data) override;

	tresult PLUGIN_API setState(IBStream* state) override;
	tresult PLUGIN_API getState(IBStream* state) override;
	uint32  PLUGIN_API getLatencySamples() override { return BlockSize - 1; }
	
	// message receiver
	tresult PLUGIN_API notify(IMessage* message) override;

	static FUnknown* createInstance(void*) { return (IAudioProcessor*)new LyngoProcessor(); }

private:

	std::mutex mutex;

	// collection of processors (one per channel)
	std::vector<std::unique_ptr<FftFir>>			  processors;

	kiss_fftr_cfg								      forward;
	std::array<std::complex<float>, IrFreqDomainSize> irFreq;
	std::array<float, ZPadBlockSize>				  irTimeZpad;
	float										      irTimeAccum;
};

