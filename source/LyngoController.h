#pragma once

#include "LyngoProcessor.h"

#include "audio/FftFir.h"

#include "external/kiss_fft130/kiss_fft.h"
#include "external/kiss_fft130/tools/kiss_fftr.h"
#include "external/vst3/public.sdk/source/vst/vsteditcontroller.h"

#include <array>
#include <memory>
#include <vector>

class LyngoEditor;

using namespace Steinberg;
using namespace Steinberg::Vst;

class LyngoController 
	: public EditControllerEx1
{
public:

	enum class IrMode
	{
		Linear   = 0,
		Exponential ,
		Smart ,
	};

	LyngoController();

	~LyngoController();

	// create function required for Plug-in factory,
	// it will be called to create new instances of this controller
	static FUnknown* createInstance (void*) { return (IEditController*)new LyngoController(); }

	// IPluginBase
	tresult PLUGIN_API initialize (FUnknown* context) override;
	tresult PLUGIN_API terminate()					 override;

	// EditController
	IPlugView* PLUGIN_API createView(const char* name) override;
	tresult    PLUGIN_API setParamNormalized(ParamID	 tag
											, ParamValue value) override;
	
	void editorAttached(EditorView* editor) override;
	void editorRemoved (EditorView* editor) override;

	tresult performEdit(ParamID id
						, ParamValue value) override;

	// message receiver
	tresult PLUGIN_API notify(IMessage* message) override;

private:
	
	std::vector<LyngoEditor*> views;
	
	// signal properties
	unsigned sampleRate;
	
	// parameters
	float fStart;
	float fEnd;

	// ir impulse response
	IrMode								irMode;
	std::array<float, IrTimeDomainSize> irTime;

	void RecalculateIrTime();
	void IrLinearChirp();
	void IrExponentialChirp();
};


