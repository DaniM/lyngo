#pragma once

#include "external/vst3/vstgui.sf/vstgui/vstcontrols.h"

#include "pluginterfaces/vst/ivstplugview.h"

#include <string>

using namespace Steinberg;
using namespace Steinberg::Vst;

class TextButton 
	: public VSTGUI::CTextLabel
{

public:
	
	TextButton(const CRect&		   size
			   , std::string&	   text
			   , CControlListener* listener = nullptr
			   , long			   id		= 0 );
	
	void SetChecked(bool checked);

	// VSTGUI::CTextLabel
	void draw(CDrawContext* pContext) override;
	CMouseEventResult onMouseDown(CPoint&       where
								  , const long& buttons) override;

private:

	bool checked;
};