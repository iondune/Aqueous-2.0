
#pragma once

#include "CWindowWidget.h"


class CCameraWindowWidget : public CWindowWidget
{

public:

	ion::CCameraController * Controller = nullptr;
	ion::Scene::CPerspectiveCamera * Camera = nullptr;

	void Draw()
	{
		vec3f Position = Controller->GetPosition();
		float Phi = Controller->GetPhi();
		float Theta = Controller->GetTheta();
		float NearPlane = Camera->GetNearPlane();
		float FarPlane = Camera->GetFarPlane();
		float FocalLength = Camera->GetFocalLength();
		float Speed = Controller->GetVelocity();
		float FOV = Camera->GetFieldOfView();

		ImGui::SetNextWindowPos(ImVec2(950, 50), ImGuiSetCond_Once);
		ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiSetCond_Once);
		ImGui::Begin("Camera", &IsVisible);
		ImGui::InputFloat3("Position", &Position.Values[0]);
		ImGui::DragFloat("Phi", &Phi, 0.01f, -1.56f, 1.56f);
		ImGui::DragFloat("Theta", &Theta, 0.01f, -3.1415f, 3.14156f);
		ImGui::InputFloat("Near Plane", &NearPlane);
		ImGui::InputFloat("Far Plane", &FarPlane);
		ImGui::InputFloat("Focal Length", &FocalLength);
		ImGui::InputFloat("Velocity", &Speed);
		ImGui::InputFloat("Field of View", &FOV);
		ImGui::End();

		Controller->SetPhi(Phi);
		Controller->SetTheta(Theta);
		Controller->SetVelocity(Speed);
		Camera->SetPosition(Position);
		Camera->SetNearPlane(NearPlane);
		Camera->SetFarPlane(FarPlane);
		Camera->SetFocalLength(FocalLength);
	}

};
