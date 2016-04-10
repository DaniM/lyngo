#include "LyngoController.h"

#include "LyngoEditor.h"
#include "LyngoUids.h"

#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/base/ibstream.h"

#include <algorithm>
#include <cassert>

namespace
{
	const float FStartMin = 1.0f;
	const float FStartMax = 100.0f;
	const float FEndMin   = 1.0f;
	const float FEndMax   = 10000.0f;
	const float MMin = 1.0f;
	const float MMax = 100.0f;
}

LyngoController::LyngoController()
	: views()
	, sampleRate(0)
	, fStart(FStartMin)
	, fEnd(FEndMin)
	, irMode(IrMode::Linear)
	, irTime()
{
	std::fill(irTime.begin(), irTime.end(), 0.0f);
}

LyngoController::~LyngoController()
{
}

tresult PLUGIN_API LyngoController::initialize(FUnknown* context)
{
	tresult result = EditControllerEx1::initialize(context);
	if (result != kResultOk)
		return result;
	
	// parameters
	const int32 flags = ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass;
	parameters.addParameter(STR16("Freq start"), 0, 0, 0.5f, flags, static_cast<int32>(Parameters::FStart));
	parameters.addParameter(STR16("Freq end")  , 0, 0, 0.5f, flags, static_cast<int32>(Parameters::FEnd));

	return kResultTrue;
}

tresult PLUGIN_API LyngoController::terminate()
{
	views.clear();
	return EditControllerEx1::terminate();
}

IPlugView* PLUGIN_API LyngoController::createView(const char* name)
{
	// someone wants my editor
	if (name && strcmp(name, "editor") == 0)
		return new LyngoEditor(this);

	return nullptr;
}

tresult PLUGIN_API LyngoController::setParamNormalized(Steinberg::Vst::ParamID	    id
													   , Steinberg::Vst::ParamValue value)
{
	// called from host to update our parameters state
	for (auto& view : views)
		view->SetParameter(id, value);

	return EditControllerEx1::setParamNormalized(id, value);
}

void LyngoController::editorAttached(EditorView* editor)
{
	views.push_back(dynamic_cast<LyngoEditor*> (editor));
}

//------------------------------------------------------------------------
void LyngoController::editorRemoved(Steinberg::Vst::EditorView* editor)
{
	views.erase(std::remove_if(views.begin()
							   , views.end()
							   , [&](LyngoEditor* view) -> bool
								 {
									return editor == view;
								 })
				, views.end());
}

tresult LyngoController::performEdit(ParamID id
									 , ParamValue value)
{
	bool impulseChanged = false;
	switch (static_cast<Parameters>(id))
	{
		case (Parameters::FStart) :
		{
			impulseChanged = true;
			fStart = FStartMin + (FStartMax - FStartMin) * value;
			break;
		}
		case (Parameters::FEnd):
		{
			impulseChanged = true;
			fEnd = FEndMin + (FEndMax - FEndMin) * value;
			break;
		}
		case (Parameters::IrMode):
		{
			impulseChanged = true;
			irMode = static_cast<IrMode>(static_cast<unsigned>(value));
			break;
		}
	}

	if (impulseChanged)
		RecalculateIrTime();

	return EditControllerEx1::performEdit(id, value);
}

void LyngoController::RecalculateIrTime()
{
	assert(sampleRate != 0);

	// recalculate fir impulse response
	switch (irMode)
	{
		case IrMode::Linear:
		{
			IrLinearChirp();
			break;
		}
		case IrMode::Exponential:
		{
			IrExponentialChirp();
			break;
		}
	}

	// notify views for reploting 
	for (auto& view : views)
		view->SetIrTimeDomain(irTime);

	// notify processor
	IMessage* message = allocateMessage();
	assert(message);
	{
		FReleaser releaser(message);
		message->setMessageID(Message::IrTime);
		message->getAttributes()->setBinary(Message::Data
											, &irTime
											, sizeof(irTime));
		sendMessage(message);
	}
}

void LyngoController::IrLinearChirp()
{
	unsigned irIndex = 0;
	float freqAccum = 0.0f;
	for (auto &irSample : irTime)
	{
		const float x = static_cast<float>(irIndex + 1) / static_cast<float>(BlockSize);
		freqAccum += (TWO_PI * (fStart + (fStart - fEnd) * x)) / static_cast<float>(sampleRate);
		irSample = sinf(freqAccum);

		++irIndex;
	}
}

void LyngoController::IrExponentialChirp()
{
	const float rap = fEnd / fStart;
	float freqAccum = 0.0f;
	unsigned irIndex = 0;
	for (auto &irSample : irTime)
	{
		const float x = static_cast<float>(irIndex + 1) / static_cast<float>(BlockSize);
		freqAccum += (TWO_PI * fStart / static_cast<float>(sampleRate)) * powf(rap, x);
		irSample = sinf(freqAccum);

		++irIndex;
	}
}

tresult PLUGIN_API LyngoController::notify(IMessage* message)
{
	if (!message)
		return kInvalidArgument;

	if (!strcmp(message->getMessageID(), Message::SampleRate))
	{
		const void* data;
		uint32      sizeRead;
		tresult     result = message->getAttributes()->getBinary(Message::Data
															     , data
															     , sizeRead);
		assert(result == kResultOk);
		sampleRate = static_cast<unsigned>(*(static_cast<const SampleRate*>(data)));
		
		RecalculateIrTime();
		
		return kResultOk;
	}

	return EditControllerEx1::notify(message);
}
