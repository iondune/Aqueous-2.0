
#pragma once

#include "CWindowWidget.h"


class CDebugWindowWidget : public CWindowWidget
{

public:

	void Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiSetCond_Once);
		ImGui::Begin("Debug", &IsVisible);
		if (ImGui::ColorEdit4("clear color", (float*)&ClearColor.Values[0], false)) ion::GL::Context::SetClearColor(ClearColor);
		if (ImGui::Button("GUI Test Window")) ShowGUITestWindow ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		if (ShowGUITestWindow)
		{
			ImGui::SetNextWindowPos(ImVec2(950, 20), ImGuiSetCond_Once);
			ImGui::ShowTestWindow(&ShowGUITestWindow);
		}
	}

	bool ShowGUITestWindow = false;
	color4f ClearColor;

};
