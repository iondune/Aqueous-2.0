
#pragma once

#include "CWindowWidget.h"


class CPointsWindowWidget : public CWindowWidget
{

public:

	void Draw()
	{
		SingletonPointer<CParticleSystem> ParticleSystem;

		ImGui::SetNextWindowPos(ImVec2(950, 50), ImGuiSetCond_Once);
		ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiSetCond_Once);
		ImGui::Begin("Points", &IsVisible);

		ImGui::Columns(3, "mycolumns");
		ImGui::Separator();
		ImGui::Text("#"); ImGui::NextColumn();
		ImGui::Text("Position"); ImGui::NextColumn();
		ImGui::Text("Color"); ImGui::NextColumn();
		ImGui::Separator();

		for (size_t i = 0; i < ParticleSystem->Particles.size(); i++)
		{
			CParticle const & p = ParticleSystem->Particles[i];

			ImGui::Text("%d", i); ImGui::NextColumn();
			ImGui::Text("%.2f, %.2f, %.2f", p.Position.X, p.Position.Y, p.Position.Z); ImGui::NextColumn();
			ImGui::Text("%.2f, %.2f, %.2f", p.Color.Red, p.Color.Green, p.Color.Blue); ImGui::NextColumn();
		}
		ImGui::Columns(1);

		ImGui::End();
	}

};
