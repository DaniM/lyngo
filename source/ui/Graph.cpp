#include "ui/Graph.h"

#include <cassert>

Graph::Graph(const CRect &size)
	: CView(size)
	, data()
{
}

void Graph::draw(CDrawContext *context)
{
	auto bounds = getViewSize();
	context->setFillColor(kGreyCColor);
	context->drawRect(bounds, kDrawFilled);

	context->setLineStyle(CLineStyle::kLineSolid);
	const float deltaX = static_cast<float>(bounds.getWidth()) / static_cast<float>(data.size());
	float x = 0.0f;
	context->setLineWidth(1);
	context->moveTo(CPoint(x, 0));
	const int halfHeight = bounds.getHeight() / 2;
	unsigned pointIndex = 0;
	for (auto &point : data)
	{
		context->lineTo(CPoint(x, bounds.top + halfHeight - (halfHeight * point)));
		assert(point <= 1.0f);

		x += deltaX;
		++pointIndex;
	}
}

void Graph::SetData(std::array<float, IrTimeDomainSize>& dataIn)
{
	data = dataIn;
	setDirty();
}