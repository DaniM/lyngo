#include "ui/TextButton.h"

TextButton::TextButton(const CRect&		   size
					   , std::string&	   text
					   , CControlListener* listener
					   , long			   id)
	: CTextLabel(size)
{
	setListener(listener);
	setTag(id);
	setText(text.c_str());
}

void TextButton::SetChecked(bool checked)
{
	this->checked = checked;
	setDirty(true);
}

void TextButton::draw(CDrawContext* pContext)
{
	pContext->setFillColor(checked ? kBlueCColor : kGreyCColor );
	pContext->setFrameColor(kWhiteCColor);
	pContext->drawRect(size, kDrawFilledAndStroked);

	//CTextLabel::draw(pContext);
}

CMouseEventResult TextButton::onMouseDown(CPoint&       where
										  , const long& buttons)
{
	if (listener)
		listener->valueChanged(this);

	return kMouseEventHandled;
}