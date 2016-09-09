
#pragma once

#include "CWindowWidget.h"
#include "CApplication.h"
#include "CGeometryClipmapsSceneObject.h"


class CGeometryClipmapsWindowWidget : public CWindowWidget
{

public:

	CGeometryClipmapsWindowWidget(CGeometryClipmapsSceneObject * Object)
	{
		this->Object = Object;
	}

	void Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiSetCond_Once);
		ImGui::Begin("Geomtry Clipmaps", &IsVisible);

		bool Checked = (Object->uDebugDisplay == 0);
		if (ImGui::Checkbox("Default", & Checked))
		{
			Object->uDebugDisplay = 0;
		}

		Checked = (Object->uDebugDisplay == 1);
		if (ImGui::Checkbox("Normals", & Checked))
		{
			Object->uDebugDisplay = 1;
		}

		Checked = (Object->uDebugDisplay == 2);
		if (ImGui::Checkbox("Color", & Checked))
		{
			Object->uDebugDisplay = 2;
		}

		Checked = (Object->uDebugDisplay == 3);
		if (ImGui::Checkbox("Diffuse", & Checked))
		{
			Object->uDebugDisplay = 3;
		}

		Checked = (Object->uDebugDisplay == 4);
		if (ImGui::Checkbox("Tex Coords", & Checked))
		{
			Object->uDebugDisplay = 4;
		}

		ImGui::Separator();

		Checked = Object->IsWireframeEnabled();
		if (ImGui::Checkbox("Wireframe", & Checked))
		{
			Object->SetWireframeEnabled(Checked);
		}

		ImGui::Separator();

		float FloatValue = Object->uOcclusionStrength;
		if (ImGui::SliderFloat("Occlusion Strength", &FloatValue, 0.0, 2.0))
		{
			Object->uOcclusionStrength = FloatValue;
		}
		FloatValue = Object->uOcclusionCap;
		if (ImGui::SliderFloat("Occlusion Cap", &FloatValue, 0.0, 1.0))
		{
			Object->uOcclusionCap = FloatValue;
		}

		ImGui::End();
	}

	CGeometryClipmapsSceneObject * Object = nullptr;

};
