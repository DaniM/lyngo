#pragma once

#include "LyngoProcessor.h"

#include "audio/FftFir.h"

#include "external/vst3/pluginterfaces/vst/ivstplugview.h"
#include "external/vst3/public.sdk/source/vst/vstguieditor.h"
#include "external/vst3/vstgui.sf/vstgui/vstcontrols.h"

#include <array>

// forward declarations
class ButtonGroup;
class Graph;
class TextButton;

using namespace Steinberg;
using namespace Steinberg::Vst;

// main editor window
class LyngoEditor
	: public CControlListener
	, public VSTGUIEditor
{
public:

	LyngoEditor(void* controller);
	~LyngoEditor();

	// Steinberg::Vst::VSTGUIEditor
	bool PLUGIN_API open (void* parent)								   override;
	void PLUGIN_API close ()										   override;

	// CControlListener
	void valueChanged           (CControl* pControl)				   override;

	// EditorView
	tresult PLUGIN_API onSize(Steinberg::ViewRect* newSize)			   override;
	tresult PLUGIN_API canResize()									   override { return kResultFalse; }
	tresult PLUGIN_API checkSizeConstraint (Steinberg::ViewRect* rect) override;

	DELEGATE_REFCOUNT(VSTGUIEditor)

	// Internal Function
	void SetParameter(ParamID	   id
					  , ParamValue value);

	void SetIrTimeDomain(std::array<float, IrTimeDomainSize>& irTimeDomain);

protected:

	CKnob*		 fStart;
	CKnob*		 fEnd;
	TextButton*  linear;
	TextButton*  exponential;
	Graph*		 graph;
};

