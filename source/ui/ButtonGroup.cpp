#include "ui/ButtonGroup.h"

ButtonGroup::ButtonGroup(const CRect&		 size
						 , CFrame&			 parent
					     , CControlListener& listener)
	: CControl(size)
	, CViewContainer(size
					 , &parent)
{
	setListener(&listener);
}

void ButtonGroup::AddButton(TextButton& button)
{
	buttons.push_back(&button);
	addView(&button);
}
