
#pragma once

#include "CWindowWidget.h"
#include "CDataManager.h"


class CPointsWindowWidget : public CWindowWidget
{

public:

	void Draw()
	{
		SingletonPointer<CDataManager> DataManager;
		CDataSet * DataSet = DataManager->GetCurrentDataSet();

		ImGui::SetNextWindowPos(ImVec2(950, 50), ImGuiSetCond_Once);
		ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiSetCond_Once);
		ImGui::Begin("Data Points", &IsVisible);

		float bbs = ParticleSystem->BillboardSize;
		ImGui::DragFloat("Point Size", &bbs, 0.01f, 0.01f, 3.f);
		ParticleSystem->BillboardSize = bbs;

		if (ImGui::DragFloat("Ramp Low", &ParticleSystem->RampLow, 0.1f, -100.f, 100.f))
		{
			ParticleSystem->UpdateColors();
		}
		if (ImGui::DragFloat("Ramp High", &ParticleSystem->RampHigh, 0.1f, -100.f, 100.f))
		{
			ParticleSystem->UpdateColors();
		}


		if (DataSet)
		{
			ImGui::Columns(DataSet->GetColumnCount() + 1, "mycolumns");
			ImGui::Separator();
			ImGui::Text("#"); ImGui::NextColumn();
			for (uint i = 0; i < DataSet->GetColumnCount(); ++ i)
			{
				ImGui::Text(DataSet->GetColumnLabel(i).c_str()); ImGui::NextColumn();
			}
			ImGui::NextColumn();
			for (uint i = 0; i < DataSet->GetColumnCount(); ++ i)
			{
				if (ImGui::Button(String::Build("X-%u", i).c_str()))
				{
					ParticleSystem->XField = i;
					ParticleSystem->SetParticlesFromData();
				}
				if (ImGui::Button(String::Build("Y-%u", i).c_str()))
				{
					ParticleSystem->YField = i;
					ParticleSystem->SetParticlesFromData();
				}
				if (ImGui::Button(String::Build("Z-%u", i).c_str()))
				{
					ParticleSystem->ZField = i;
					ParticleSystem->SetParticlesFromData();
				}
				ImGui::NextColumn();
			}
			ImGui::Separator();

			int NumLines = (int) DataSet->Size();
			int display_start, display_end;
			ImGui::CalcListClipping(NumLines, ImGui::GetTextLineHeight(), &display_start, &display_end);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (display_start) * ImGui::GetTextLineHeight());
			for (int i = display_start; i < display_end; i++)
			{
				CDataRow const & Row = DataSet->GetRow(i);

				ImGui::Text("%d", i); ImGui::NextColumn();
				for (uint j = 0; j < DataSet->GetColumnCount(); ++ j)
				{
					ImGui::Text("%s", Row.GetFieldAsString(j).c_str()); ImGui::NextColumn();
				}
			}
			ImGui::Columns(1);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (NumLines - display_end) * ImGui::GetTextLineHeight());
		}

		ImGui::End();
	}

	CParticleSystem * ParticleSystem = nullptr;

};
