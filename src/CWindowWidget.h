
#pragma once

#include <ionEngine.h>


class CWindowWidget
{

public:

	virtual void Draw() = 0;

	void DrawIfVisible();
	void ToggleVisibility();

protected:

	bool IsVisible = false;

};
