#include "LyngoEditor.h"
#include "LyngoController.h"
#include "LyngoUids.h"

#include "base/source/fstring.h"

#include "ui/ButtonGroup.h"
#include "ui/Graph.h"
#include "ui/TextButton.h"

#include <cassert>
#include <stdio.h>
#include <string>   
#include <math.h>

namespace
{
	// UI size
	const int EditorWidth  = 350;
	const int EditorHeight = 140;
	const int ButtonHeight = 20;
};

// main editor
LyngoEditor::LyngoEditor(void* controller)
	: VSTGUIEditor(controller)
	, fStart(nullptr)
	, fEnd(nullptr)
{
	setIdleRate(50); // 1000ms/50ms = 20Hz

	// set the default View size
	ViewRect viewRect(0, 0, EditorWidth, EditorHeight);
	setRect(viewRect);
}

LyngoEditor::~LyngoEditor()
{
}

tresult PLUGIN_API LyngoEditor::onSize(ViewRect* newSize)
{
	return VSTGUIEditor::onSize(newSize);
}

tresult PLUGIN_API LyngoEditor::checkSizeConstraint(ViewRect* rect)
{
	if (rect->right - rect->left < EditorWidth)
	{
		rect->right = rect->left + EditorWidth;
	}
	else if (rect->right - rect->left > EditorWidth + 50)
	{
		rect->right = rect->left + EditorWidth + 50;
	}
	if (rect->bottom - rect->top < EditorHeight)
	{
		rect->bottom = rect->top + EditorHeight;
	}
	else if (rect->bottom - rect->top > EditorHeight + 50)
	{
		rect->bottom = rect->top + EditorHeight + 50;
	}
	return kResultTrue;
}

bool LyngoEditor::open(void* parent)
{
	if (frame) // already attached!
		return false;

	CRect editorSize(0, 0, EditorWidth, EditorHeight);

	frame = new CFrame(editorSize, parent, this);
	frame->setBackgroundColor(kBlackCColor);

	CBitmap* knobBg     = new CBitmap("KnobBackground.png");
	CBitmap* knobHandle = new CBitmap("KnobHandle.png");

	int y = 0;
	CRect bounds(0, y, knobBg->getWidth(), knobBg->getHeight());
	fStart = new CKnob(bounds
					   , this
		               , static_cast<int>(Parameters::FStart)
					   , knobBg
					   , knobHandle
					   , CPoint(0, 0));
	frame->addView(fStart);

	const int xMargin = 15;
	bounds.offset(knobBg->getWidth() + xMargin, 0);
	fEnd = new CKnob(bounds
					 , this
					 , static_cast<int>(Parameters::FEnd)
					 , knobBg
					 , knobHandle
					 , CPoint(0, 0));
	frame->addView(fEnd);

	y += bounds.getHeight();
	const int modeButonWidth = EditorWidth / 3;
	CRect buttonBounds(0, y, modeButonWidth, y + ButtonHeight);
	linear = new TextButton(buttonBounds, std::string("LINEAR"), this, static_cast<int>(Parameters::IrLinear));
	frame->addView(linear);

	buttonBounds.offset(modeButonWidth, 0);
	exponential = new TextButton(buttonBounds, std::string("EXPONENTIAL"), this, static_cast<int>(Parameters::IrExponential));
	frame->addView(exponential);

	y += buttonBounds.getHeight();
	const CRect graphBounds(0, y , EditorWidth, EditorHeight);
	graph = new Graph(graphBounds);
	frame->addView(graph);

	knobBg    ->forget();
	knobHandle->forget();

	// sync UI controls with controller parameter values
	ParamValue fStartValue = getController()->getParamNormalized(static_cast<ParamID>(Parameters::FStart));
	ParamValue fEndValue   = getController()->getParamNormalized(static_cast<ParamID>(Parameters::FEnd));
	ParamValue irModeValue = getController()->getParamNormalized(static_cast<ParamID>(Parameters::IrMode));
	SetParameter(static_cast<ParamID>(Parameters::FStart), fStartValue);
	SetParameter(static_cast<ParamID>(Parameters::FEnd)  , fEndValue);
	SetParameter(static_cast<ParamID>(Parameters::IrMode), irModeValue);

	return true;
}

//------------------------------------------------------------------------
void PLUGIN_API LyngoEditor::close()
{
	if (frame)
	{
		delete frame;
		frame = nullptr;
	}
	
	fStart		= nullptr;
	fEnd		= nullptr;
	graph		= nullptr;
	linear		= nullptr;
	exponential = nullptr;
}

//------------------------------------------------------------------------
void LyngoEditor::valueChanged(CControl* control)
{
	switch (static_cast<Parameters>(control->getTag()))
	{
		case Parameters::IrLinear:
		{
			controller->setParamNormalized(static_cast<ParamID>(Parameters::IrMode), static_cast<ParamValue>(LyngoController::IrMode::Linear));
			controller->performEdit		  (static_cast<ParamID>(Parameters::IrMode), static_cast<ParamValue>(LyngoController::IrMode::Linear));
			return;
			break;
		}
		case Parameters::IrExponential:
		{
			controller->setParamNormalized(static_cast<ParamID>(Parameters::IrMode), static_cast<ParamValue>(LyngoController::IrMode::Exponential));
			controller->performEdit		  (static_cast<ParamID>(Parameters::IrMode), static_cast<ParamValue>(LyngoController::IrMode::Exponential));
			return;
			break;
		}
		case Parameters::IrSmart:
		{
			controller->setParamNormalized(static_cast<ParamID>(Parameters::IrMode), static_cast<ParamValue>(LyngoController::IrMode::Smart));
			controller->performEdit		  (static_cast<ParamID>(Parameters::IrMode), static_cast<ParamValue>(LyngoController::IrMode::Smart));
			return;
			break;
		}
		default:
			break;
	}

	controller->setParamNormalized(static_cast<ParamID>(control->getTag()), control->getValue());
	controller->performEdit		  (static_cast<ParamID>(control->getTag()), control->getValue());
}

void LyngoEditor::SetParameter(ParamID      id
						       , ParamValue value)
{
	switch (id)
	{
		case (static_cast<ParamID>(Parameters::FStart)):
		{
			fStart->setValue(value);
			break;
		}
		case (static_cast<ParamID>(Parameters::FEnd)):
		{
			fEnd->setValue(value);
			break;
		}
		case (static_cast<ParamID>(Parameters::IrMode)):
		{
			LyngoController::IrMode mode = static_cast<LyngoController::IrMode>(static_cast<unsigned>(value));
			linear	   ->SetChecked(mode == LyngoController::IrMode::Linear);
			exponential->SetChecked(mode == LyngoController::IrMode::Exponential);
			break;
		}
		default:
			break;
	}
}

void LyngoEditor::SetIrTimeDomain(std::array<float, IrTimeDomainSize>& irTime)
{
	graph->SetData(irTime);
}

