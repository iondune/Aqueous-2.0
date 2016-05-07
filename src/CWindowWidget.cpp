
#include "CWindowWidget.h"


void CWindowWidget::DrawIfVisible()
{
	if (IsVisible)
	{
		Draw();
	}
}

void CWindowWidget::ToggleVisibility()
{
	IsVisible = ! IsVisible;
}
