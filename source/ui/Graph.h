#pragma once

#include "LyngoUids.h"

#include "external/vst3/vstgui.sf/vstgui/vstcontrols.h"

#include "pluginterfaces/vst/ivstplugview.h"

#include <array>

using namespace Steinberg;
using namespace Steinberg::Vst;

// graph plot view
class Graph
	: public CView
{

public:

	Graph(const CRect &size);

	void SetData(std::array<float, IrTimeDomainSize>& data);

	// CView
	void draw(CDrawContext* context) override;

private:

	std::array<float, IrTimeDomainSize> data;
};