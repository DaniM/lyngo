#include "LyngoProcessor.h"

#include "LyngoUids.h"

#include "audio/FftFir.h"

#include "utils/RingBuffer.h"

#include <cassert>
#include <string>
#include <thread>

LyngoProcessor::LyngoProcessor()
	: processors()
	, forward(nullptr)
	, irFreq()
	, irTimeZpad()
	, irTimeAccum(0.0f)
{
	setControllerClass(LyngoControllerUid);

	forward = kiss_fftr_alloc(ZPadBlockSize, 0, 0, 0);
	std::fill(irTimeZpad.begin(), irTimeZpad.end(), 0.0f);
}

LyngoProcessor::~LyngoProcessor()
{
	free(forward);
}

tresult PLUGIN_API LyngoProcessor::initialize(FUnknown* context)
{
	tresult result = AudioEffect::initialize(context);
	if (result == kResultTrue)
	{
		addAudioInput (STR16("AudioInput") , SpeakerArr::kStereo);
		addAudioOutput(STR16("AudioOutput"), SpeakerArr::kStereo);
	}
	return result;
}

tresult PLUGIN_API LyngoProcessor::setupProcessing(ProcessSetup& setup)
{
	// notify controller 
	IMessage* message = allocateMessage();
	assert(message);
	{
		FReleaser releaser(message);
		message->setMessageID(Message::SampleRate);
		message->getAttributes()->setBinary(Message::Data
											, &setup.sampleRate
											, sizeof(setup.sampleRate));
		sendMessage(message);
	}
	
	setup.maxSamplesPerBlock = BlockSize;

	return kResultTrue;
}

tresult PLUGIN_API LyngoProcessor::setBusArrangements(SpeakerArrangement*   inputs
													  , int32				numIns
													  , SpeakerArrangement* outputs
													  , int32	 		    numOuts)
{
	// TODO
	// we only support one in and output bus and these buses must have the same number of channels
	//if (numIns == 1 && numOuts == 1 && inputs[0] == outputs[0])
	return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
}

tresult PLUGIN_API LyngoProcessor::setActive(TBool state)
{
	SpeakerArrangement arr;
	if (getBusArrangement(kOutput, 0, arr) != kResultTrue)
		return kResultFalse;

	const unsigned noOfChannels = static_cast<unsigned>(SpeakerArr::getChannelCount(arr));
	if (noOfChannels == 0)
		return kResultFalse;

	if (state)
	{
		for (unsigned channel = 0; channel < noOfChannels; ++channel)
			processors.push_back(std::unique_ptr<FftFir>(new FftFir(irFreq, irTimeAccum)));
	}
	else
	{
		processors.clear();
	}

	return AudioEffect::setActive(state);
}

tresult PLUGIN_API LyngoProcessor::process(ProcessData& data)
{
	if (data.numSamples <= 0)
		return kResultTrue;

	unsigned chIndex = 0;
	std::lock_guard<std::mutex> lock(mutex);
	for (auto &processor : processors)
	{
		float* in  = data.inputs [0].channelBuffers32[chIndex];
		float* out = data.outputs[0].channelBuffers32[chIndex];
		processor->Process(in, out, data.numSamples);
		++chIndex;
	}

	return kResultTrue;
}

tresult PLUGIN_API LyngoProcessor::setState(IBStream* state)
{
	// TODO
	/*
	if (!state)
		return kResultFalse;

	// called when we load a preset, the model has to be reloaded

	float savedDelay = 0.f;
	if (state->read(&savedDelay, sizeof(float)) != kResultOk)
	{
		return kResultFalse;
	}

	int32 savedBypass = 0;
	if (state->read(&savedBypass, sizeof(int32)) != kResultOk)
	{
		// could be an old version, continue 
	}

#if BYTEORDER == kBigEndian
	SWAP_32(savedDelay)
		SWAP_32(savedBypass)
#endif

		mDelay = savedDelay;
	mBypass = savedBypass > 0;
	*/

	return kResultOk;
}

tresult PLUGIN_API LyngoProcessor::getState(IBStream* state)
{
	// TODO
	/*
	// here we need to save the model

	float toSaveDelay = mDelay;
	int32 toSaveBypass = mBypass ? 1 : 0;

#if BYTEORDER == kBigEndian
	SWAP_32(toSaveDelay)
		SWAP_32(toSaveBypass)
#endif

		state->write(&toSaveDelay, sizeof(float));
	state->write(&toSaveBypass, sizeof(int32));
	*/

	return kResultOk;
}

tresult PLUGIN_API LyngoProcessor::notify(IMessage* message)
{
	if (!message)
		return kInvalidArgument;

	if (!strcmp(message->getMessageID(), Message::IrTime))
	{
		const void* data;
		uint32      sizeRead;
		tresult     result = message->getAttributes()->getBinary(Message::Data
																 , data
																 , sizeRead);
		assert(result == kResultOk);

		std::lock_guard<std::mutex> lock(mutex);
		
		// get message		
		const std::array<float, IrTimeDomainSize>& irTime = *(reinterpret_cast<const std::array<float, IrTimeDomainSize>*>(data));
		
		// zero pad
		std::copy(irTime.begin(), irTime.end(), irTimeZpad.begin());

		// Fft
		kiss_fftr(forward, &irTimeZpad.front(), reinterpret_cast<kiss_fft_cpx*>(&irFreq.front()));

		// recalculate ir accum
		irTimeAccum = 0.0f;
		for (auto &irSample : irTime)
			irTimeAccum += irSample;

		return kResultOk;
	}

	return AudioEffect::notify(message);
}



