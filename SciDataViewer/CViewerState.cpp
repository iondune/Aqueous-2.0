
#include "CViewerState.h"
#include "CApplication.h"
#include "CParticleSystem.h"
#include "CCameraWindowWidget.h"
#include "CPointsWindowWidget.h"
#include "CDebugWindowWidget.h"
#include "CWaterSurfaceSceneObject.h"


using namespace ion;
using namespace ion::Graphics;
using namespace ion::Scene;

void CViewerState::Init()
{
	RenderPass = new CRenderPass(Application->Context);
	RenderPass->SetRenderTarget(Application->RenderTarget);
	SceneManager->AddRenderPass(RenderPass);

	DebugCamera = new CPerspectiveCamera(Application->GetWindow()->GetAspectRatio());
	DebugCamera->SetPosition(vec3f(-10, 10, 0));
	DebugCamera->SetFarPlane(50000.f);
	RenderPass->SetActiveCamera(DebugCamera);

	DebugCameraControl = new CGamePadCameraController(DebugCamera);
	DebugCameraControl->SetVelocity(10.f);
	DebugCameraControl->SetTheta(0);
	DebugCameraControl->SetPhi(-Constants32::Pi / 4.f);
	AddListener(DebugCameraControl);

	CSimpleMeshSceneObject * Cube = new CSimpleMeshSceneObject();
	Cube->SetShader(Application->DiffuseShader);
	Cube->SetMesh(Application->CubeMesh);
	RenderPass->AddSceneObject(Cube);

	CPointLight * Light = new CPointLight();
	Light->SetPosition(vec3f(-128, 256, 128));
	Light->SetRadius(150.f);
	RenderPass->AddLight(Light);

	SharedPointer<ITextureCubeMap> SkyBoxTexture = AssetManager->LoadCubeMapTexture(
		"TropicalSunnyDayLeft2048.png",
		"TropicalSunnyDayRight2048.png",
		"TropicalSunnyDayUp2048.png",
		"TropicalSunnyDayDown2048.png",
		"TropicalSunnyDayFront2048.png",
		"TropicalSunnyDayBack2048.png");

	SkyBox = new CSimpleMeshSceneObject();
	SkyBox->SetMesh(Application->CubeMesh);
	SkyBox->SetShader(Application->SkyBoxShader);
	SkyBox->SetTexture("uTexture", SkyBoxTexture);
	RenderPass->AddSceneObject(SkyBox);

	//ParticleSystem = new CParticleSystem(Application->ParticleShader);
	//RenderPass->AddSceneObject(ParticleSystem);

	//for (int x = 1; x < 20; ++ x)
	//for (int y = 1; y < 20; ++ y)
	//for (int z = 1; z < 20; ++ z)
	//{
	//	static int const NumColors = 7;
	//	static color3f const ColorChoices[] = {Colors::Red, Colors::Green, Colors::Blue, Colors::Cyan, Colors::Magenta, Colors::Yellow, Colors::Orange};
	//	CParticle p;
	//	p.Color = ColorChoices[rand() % NumColors];
	//	p.Position = vec3f((f32) x, (f32) y, (f32) z);

	//	ParticleSystem->Particles.push_back(p);
	//}
	//ParticleSystem->Update();

	WaterSurface = new CWaterSurfaceSceneObject();
	WaterSurface->SkyBoxTexture = SkyBoxTexture;
	RenderPass->AddSceneObject(WaterSurface);

	DebugWindow = new CDebugWindowWidget();
	DebugWindow->RenderTarget = Application->RenderTarget;
	DebugWindow->RenderTarget->SetClearColor(DebugWindow->ClearColor = color3f(0.65f, 0.85f, 0.95f));

	CameraWindow = new CCameraWindowWidget();
	CameraWindow->Camera = DebugCamera;
	CameraWindow->Controller = DebugCameraControl;

	PointsWindow = new CPointsWindowWidget();
	PointsWindow->ParticleSystem = ParticleSystem;
}

void CViewerState::Update(float const Elapsed)
{
	DebugCameraControl->Update(Elapsed);

	SkyBox->SetPosition(RenderPass->GetActiveCamera()->GetPosition());

	GUI();
}

void CViewerState::GUI()
{
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
			if (ImGui::MenuItem("Water")) { WaterSurface->ToggleGUI(); };
			if (ImGui::MenuItem("Debug")) { DebugWindow->ToggleVisibility(); };
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	//DebugWindow->DrawIfVisible();
	//CameraWindow->DrawIfVisible();
	//PointsWindow->DrawIfVisible();
	WaterSurface->GUI();
}

void CViewerState::OnEvent(IEvent & Event)
{
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

			case EKey::M:
				WaterSurface->ToggleGUI();
				break;

			case EKey::F1:
				DebugWindow->ToggleVisibility();
				break;

			case EKey::I:
				if (Application->GetWindow()->IsKeyDown(EKey::LeftControl) || Application->GetWindow()->IsKeyDown(EKey::RightControl))
				{
					DoImport();
				}
				break;
			}
		}
	}
}

#include <nfd.h>

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
