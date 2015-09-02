
#include "CViewerState.h"
#include "CApplication.h"
#include "CParticleSystem.h"
#include "CCameraWindowWidget.h"
#include "CPointsWindowWidget.h"
#include "CDebugWindowWidget.h"


void CViewerState::Init()
{
	DebugCamera = SceneManager->GetFactory()->AddPerspectiveCamera(Application->GetWindow()->GetAspectRatio());
	DebugCameraControl = new CGamePadCameraController(DebugCamera);
	DebugCamera->SetPosition(vec3f(-10, 10, 0));
	DebugCameraControl->SetVelocity(10.f);
	DebugCameraControl->SetTheta(0);
	DebugCameraControl->SetPhi(-Constants32::Pi / 4.f);
	DebugCamera->SetFarPlane(50000.f);
	AddListener(DebugCameraControl);
	SceneManager->GetScene()->SetActiveCamera(DebugCamera);

	SceneManager->GetFactory()->AddMeshNode("Cube", "Diffuse");
	ILightSceneNode * Light = SceneManager->GetFactory()->AddLight(vec3f(-128, 256, 128));
	Light->SetRadius(150.f);

	ParticleSystem->Init();

	for (int x = 1; x < 20; ++ x)
	for (int y = 1; y < 20; ++ y)
	for (int z = 1; z < 20; ++ z)
	{
		static int const NumColors = 7;
		static color3f const ColorChoices[] = {Colors::Red, Colors::Green, Colors::Blue, Colors::Cyan, Colors::Magenta, Colors::Yellow, Colors::Orange};
		CParticle p;
		p.Color = ColorChoices[rand() % NumColors];
		p.Position = vec3f((f32) x, (f32) y, (f32) z);

		ParticleSystem->Particles.push_back(p);
	}
	ParticleSystem->Update();

	ClearColor = color3f(0.65f, 0.85f, 0.95f);
	ion::GL::Context::SetClearColor(ClearColor);

	GUIManager->Init(Application->GetWindow(), true);
	GUIManager->AddFontFromFile("Assets/Fonts/OpenSans.ttf", 30.f);

	CameraWindow = new CCameraWindowWidget();
	CameraWindow->Camera = DebugCamera;
	CameraWindow->Controller = DebugCameraControl;

	PointsWindow = new CPointsWindowWidget();

	DebugWindow = new CDebugWindowWidget();
	DebugWindow->ClearColor = ion::GL::Context::GetClearColor();
}

void CViewerState::Update(float const Elapsed)
{
	DebugCameraControl->Update(Elapsed);
	ParticleSystem->Draw();

	GUI();
}

#include <nfd.h>


void CViewerState::GUI()
{
	GUIManager->NewFrame();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Import", "Ctrl+I"))
			{
				DoImport();
			}
			if (ImGui::BeginMenu("Export..."))
			{
				if (ImGui::MenuItem("OBJ"))
				{
					DoExportOBJ();
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Quit", "Alt+F4")) { Application->Close(); }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("Camera")) { CameraWindow->ToggleVisibility(); };
			if (ImGui::MenuItem("Points")) { PointsWindow->ToggleVisibility(); };
			if (ImGui::MenuItem("Debug")) { DebugWindow->ToggleVisibility(); };
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	DebugWindow->DrawIfVisible();
	CameraWindow->DrawIfVisible();
	PointsWindow->DrawIfVisible();
}

void CViewerState::OnEvent(IEvent & Event)
{
	GUIManager->OnEvent(Event);

	if (GUIManager->IO.WantCaptureMouse)
	{
		Event.Block();
	}

	if (InstanceOf<SKeyboardEvent>(Event))
	{
		SKeyboardEvent & KeyboardEvent = As<SKeyboardEvent>(Event);
		
		if (! KeyboardEvent.Pressed)
		{
			switch (KeyboardEvent.Key)
			{
			case EKey::Grave:
				break;

			case EKey::C:
				CameraWindow->ToggleVisibility();
				break;

			case EKey::P:
				PointsWindow->ToggleVisibility();
				break;

			case EKey::F1:
				DebugWindow->ToggleVisibility();
				break;

			case EKey::I:
				if (Application->GetWindow()->IsKeyDown(EKey::LeftControl) || Application->GetWindow()->IsKeyDown(EKey::RightControl))
				{
					DoImport();
				}
			}
		}
	}
}

void CViewerState::DoImport()
{
	nfdchar_t *outPath = NULL;
	nfdresult_t result = NFD_OpenDialog(NULL, nullptr/*"C:/Users/Ian/Dropbox/Projects/ICEX 2015/Data"*/, &outPath);

	if (result == NFD_OKAY) {
		puts(outPath);
		DataManager->Load(outPath);
		ParticleSystem->SetParticlesFromData();

		free(outPath);
	}
	else if (result == NFD_CANCEL) {
		puts("User pressed cancel.");
	}
	else {
		printf("Error: %s\n", NFD_GetError());
	}
}

void CViewerState::DoExportOBJ()
{
	nfdchar_t *outPath = NULL;
	nfdresult_t result = NFD_SaveDialog("*.obj", nullptr, &outPath);

	if (result == NFD_OKAY) {
		puts(outPath);
		DataManager->WriteOBJ(outPath);

		free(outPath);
	}
	else if (result == NFD_CANCEL) {
		puts("User pressed cancel.");
	}
	else {
		printf("Error: %s\n", NFD_GetError());
	}
}
