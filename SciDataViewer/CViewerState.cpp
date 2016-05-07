
#include "CViewerState.h"
#include "CApplication.h"
#include "CParticleSystem.h"
#include "CCameraWindowWidget.h"
#include "CPointsWindowWidget.h"
#include "CDebugWindowWidget.h"

#include "CWaterSurfaceSceneObject.h"
#include "CVolumeSceneObject.h"


using namespace ion;
using namespace ion::Graphics;
using namespace ion::Scene;

void CViewerState::Init()
{
	SingletonPointer<ion::CAssetManager> AssetManager;
	
	SceneFrameBuffer = Application->Context->CreateFrameBuffer();
	SceneColor = GraphicsAPI->CreateTexture2D(Application->GetWindow()->GetSize(), ITexture::EMipMaps::False, ITexture::EFormatComponents::RGBA, ITexture::EInternalFormatType::Fix8);
	SceneDepth = GraphicsAPI->CreateTexture2D(Application->GetWindow()->GetSize(), ITexture::EMipMaps::False, ITexture::EFormatComponents::R, ITexture::EInternalFormatType::Depth);
	SceneFrameBuffer->AttachColorTexture(SceneColor, 0);
	SceneFrameBuffer->AttachDepthTexture(SceneDepth);
	if (! SceneFrameBuffer->CheckCorrectness())
	{
		Log::Error("Frame buffer not valid!");
	}

	DefaultRenderPass = new CRenderPass(Application->Context);
	DefaultRenderPass->SetRenderTarget(SceneFrameBuffer);
	SceneManager->AddRenderPass(DefaultRenderPass);

	VolumeRenderPass = new CRenderPass(Application->Context);
	VolumeRenderPass->SetRenderTarget(SceneFrameBuffer);
	SceneManager->AddRenderPass(VolumeRenderPass);

	SharedPointer<IFrameBuffer> CopyFrameBuffer = Application->Context->CreateFrameBuffer();
	SwapColor = GraphicsAPI->CreateTexture2D(Application->GetWindow()->GetSize(), ITexture::EMipMaps::False, ITexture::EFormatComponents::RGBA, ITexture::EInternalFormatType::Fix8);
	SwapColor->SetWrapMode(ITexture::EWrapMode::Clamp);
	CopyFrameBuffer->AttachColorTexture(SwapColor, 0);
	if (! CopyFrameBuffer->CheckCorrectness())
	{
		Log::Error("Frame buffer not valid!");
	}

	CopyRenderPass = new CRenderPass(Application->Context);
	CopyRenderPass->SetRenderTarget(CopyFrameBuffer);
	SceneManager->AddRenderPass(CopyRenderPass);

	WaterRenderPass = new CRenderPass(Application->Context);
	WaterRenderPass->SetRenderTarget(SceneFrameBuffer);
	SceneManager->AddRenderPass(WaterRenderPass);

	FinalRenderPass = new CRenderPass(Application->Context);
	FinalRenderPass->SetRenderTarget(Application->RenderTarget);
	SceneManager->AddRenderPass(FinalRenderPass);

	DebugCamera = new CPerspectiveCamera(Application->GetWindow()->GetAspectRatio());
	DebugCamera->SetPosition(vec3f(-10, 10, 0));
	DebugCamera->SetFarPlane(50000.f);
	DefaultRenderPass->SetActiveCamera(DebugCamera);
	VolumeRenderPass->SetActiveCamera(DebugCamera);
	WaterRenderPass->SetActiveCamera(DebugCamera);

	DebugCameraControl = new CGamePadCameraController(DebugCamera);
	DebugCameraControl->SetVelocity(10.f);
	DebugCameraControl->SetTheta(0);
	DebugCameraControl->SetPhi(-Constants32::Pi / 4.f);
	AddListener(DebugCameraControl);

	//CSimpleMeshSceneObject * Cube = new CSimpleMeshSceneObject();
	//Cube->SetShader(Application->DiffuseShader);
	//Cube->SetMesh(Application->CubeMesh);
	//RenderPass->AddSceneObject(Cube);

	CPointLight * Light = new CPointLight();
	Light->SetPosition(vec3f(-128, 256, 128));
	Light->SetRadius(150.f);
	DefaultRenderPass->AddLight(Light);

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
	DefaultRenderPass->AddSceneObject(SkyBox);



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
	WaterSurface->SceneColor = SwapColor;
	WaterRenderPass->AddSceneObject(WaterSurface);

	Volume = new CVolumeSceneObject();
	SharedPointer<Graphics::ITexture3D> VolumeData = GraphicsAPI->CreateTexture3D(vec3u(30), ITexture::EMipMaps::False, ITexture::EFormatComponents::RGBA, ITexture::EInternalFormatType::Fix8);
	byte * Data = new byte[30 * 30 * 30 * 4];
	for (int k = 0; k < 30; ++ k)
	{
		for (int j = 0; j < 30; ++ j)
		{
			for (int i = 0; i < 30; ++ i)
			{
				color4i const Color = color4f(i / 30.f, j / 30.f, k / 30.f, i / 90.f + j / 90.f + k / 90.f);
				for (int t = 0; t < 4; ++ t)
				{
					Data[(i + j * 30 + k * 30 * 30) * 4 + t] = Color[t];
				}
			}
		}
	}
	VolumeData->Upload(Data, vec3u(30), ITexture::EFormatComponents::RGBA, EScalarType::UnsignedInt8);
	delete[] Data;
	VolumeData->SetMinFilter(ITexture::EFilter::Linear);
	VolumeData->SetMagFilter(ITexture::EFilter::Linear);
	VolumeData->SetWrapMode(ITexture::EWrapMode::Clamp);
	Volume->VolumeData = VolumeData;
	Volume->SetScale(vec3f(2560.f, 160.f, 2560.f));
	Volume->SetPosition(vec3f(1280.f, -160.f, 1280.f));
	VolumeRenderPass->AddSceneObject(Volume);


	CSimpleMeshSceneObject * PostProcessObject = new CSimpleMeshSceneObject();
	PostProcessObject->SetMesh(CGeometryCreator::CreateScreenTriangle());
	PostProcessObject->SetShader(Application->QuadCopyShader);
	PostProcessObject->SetTexture("uTexture", SceneColor);
	CopyRenderPass->AddSceneObject(PostProcessObject);
	FinalRenderPass->AddSceneObject(PostProcessObject);

	DebugWindow = new CDebugWindowWidget();
	DebugWindow->RenderTarget = Application->RenderTarget;
	DebugWindow->RenderTarget->SetClearColor(DebugWindow->ClearColor = color3f(0.65f, 0.85f, 0.95f));

	CameraWindow = new CCameraWindowWidget();
	CameraWindow->Camera = DebugCamera;
	CameraWindow->Controller = DebugCameraControl;

	PointsWindow = new CPointsWindowWidget();
	PointsWindow->ParticleSystem = ParticleSystem;
	//Shark
	SharkObject = new SharkSceneObject("Assets/Models/leopardSharkUnparented.dae");
	SharkObject->SetShader(Application->SharkShader);
	SharkObject->TriggerReload();
	RenderPass->AddSceneObject(SharkObject);

	//Spline
	Spline = new KeySpline();
	glm::vec3 offset (0,0,0);
	Spline->addNode(SplineNode(offset+glm::vec3(-3,0,5)));
   Spline->addNode(SplineNode(offset+glm::vec3(1,0,2)));
   Spline->addNode(SplineNode(offset+glm::vec3(2,0,-4)));
   Spline->addNode(SplineNode(offset+glm::vec3(-3,0,-8)));
	Spline->addNode(SplineNode(offset+glm::vec3(-3,0,5)));
   Spline->addNode(SplineNode(offset+glm::vec3(1,0,2)));
   Spline->addNode(SplineNode(offset+glm::vec3(2,0,-4)));
   Spline->addNode(SplineNode(offset+glm::vec3(-3,0,-8)));
	Spline->addNode(SplineNode(offset+glm::vec3(-3,0,5)));
   Spline->addNode(SplineNode(offset+glm::vec3(1,0,2)));
   Spline->addNode(SplineNode(offset+glm::vec3(2,0,-4)));
   Spline->addNode(SplineNode(offset+glm::vec3(-3,0,-8)));
  
   Spline->addNode(SplineNode(glm::vec3(0,0,-10)));
}

void CViewerState::Update(float const Elapsed)
{
	SceneFrameBuffer->ClearColorAndDepth();

	DebugCameraControl->Update(Elapsed);

	SkyBox->SetPosition(DefaultRenderPass->GetActiveCamera()->GetPosition());
	SharkObject->update(*Spline, Elapsed);
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
			if (ImGui::MenuItem("Volume")) { Volume->ToggleGUI(); };
			if (ImGui::MenuItem("Debug")) { DebugWindow->ToggleVisibility(); };

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	DebugWindow->DrawIfVisible();
	CameraWindow->DrawIfVisible();
	PointsWindow->DrawIfVisible();
	WaterSurface->GUI();
	Volume->GUI();
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

			case EKey::N:
				Volume->ToggleGUI();
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
