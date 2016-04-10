#pragma once

#include "external/vst3/vstgui.sf/vstgui/vstcontrols.h"

#include "pluginterfaces/vst/ivstplugview.h"

#include <string>
#include <vector>

// forward declarations
class TextButton;

using namespace Steinberg;
using namespace Steinberg::Vst;

class ButtonGroup 
	: public CControl
	, public CViewContainer
{

public:
	
	ButtonGroup(const CRect&		size
				, CFrame&			parent
			    , CControlListener& listener);
	
	void AddButton(TextButton& button);

private:

	std::vector<TextButton*> buttons;
};