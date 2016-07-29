
#include "CViewerState.h"
#include "CApplication.h"
#include "CParticleSystem.h"
#include "CCameraWindowWidget.h"
#include "CPointsWindowWidget.h"
#include "CDebugWindowWidget.h"

#include "CWaterSurfaceSceneObject.h"
#include "CVolumeSceneObject.h"
#include "ColorTable.h"
#include "UTMtoLatLong.h"


using namespace ion;
using namespace ion::Graphics;
using namespace ion::Scene;

vector<double> LoadData(string const & FileName)
{
	vector<double> Elements;

	string const FileContents = File::ReadAsString(FileName);
	vector<string> const ElementStrings = String::Explode(FileContents, ',');
	std::transform(ElementStrings.begin(), ElementStrings.end(), std::back_inserter(Elements), [](string const & s) -> double
	{
		return atof(s.c_str());
	});

	double const Max = *std::max_element(Elements.begin(), Elements.end());
	double Min = Max;
	std::for_each(Elements.begin(), Elements.end(), [&Min](double const & Element)
	{
		if (Element != 0)
		{
			Min = Minimum(Element, Min);
		}
	});

	std::for_each(Elements.begin(), Elements.end(), [Max, Min](double & Element)
	{
		Element = Clamp((Element - Min) / (Max - Min), 0.0, 1.0);
	});

	return Elements;
}

vector<vec3f> LoadPointsFromTxt(string const & FileName, bool commas)
{
	vector<vec3f> Points;

	std::ifstream file(FileName);

	if (! file.is_open())
	{
		cerr << FileName << " could not be opened." << endl;
	}

	double northing, easting, elev;
	char comma;

	if (commas)
	{
		while (file >> northing >> comma >> easting >> comma >> elev)
		{
			vec2d const latlong = UTMToLatLon(northing, easting, 11, true);
			Points.push_back(vec3d(latlong.X, latlong.Y, elev));
			Points.back().Z /= 10.f;
		}
	}
	else
	{
		while (file >> northing >> easting >> elev)
		{
			vec2d const latlong = UTMToLatLon(northing, easting, 11, true);
			Points.push_back(vec3d(latlong.X, latlong.Y, elev));
			Points.back().Z /= 10.f;
		}
	}

	if (Points.size() == 0)
	{
		cout << FileName << " has no points." << endl;
	}

	return Points;
}

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

	CopyFrameBuffer = Application->Context->CreateFrameBuffer();
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
	//DefaultRenderPass->AddSceneObject(Cube);

	CPointLight * Light = new CPointLight();
	Light->SetPosition(vec3f(-128, 256, 128));
	Light->SetRadius(150.f);
	DefaultRenderPass->AddLight(Light);

#ifdef ION_CONFIG_RELEASE
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
#endif

	ParticleSystem = new CParticleSystem(Application->ParticleShader);
	DefaultRenderPass->AddSceneObject(ParticleSystem);

	auto PointsToParticles = [this](string const & FileName, color3i const & Color, bool Commas)
	{
		vector<vec3f> Points = LoadPointsFromTxt(FileName, Commas);

		for (size_t i = 0; i < Points.size(); i += 20)
		{
			vec3f const & Point = Points[i];
			CParticle p;
			p.Color = Color;
			p.Position = vec3f((Point.X - 33.3) * 800, -Point.Z / 5.0, (Point.Y + 118.3) * 800);

			ParticleSystem->Particles.push_back(p);
		}
	};

	PointsToParticles("Data/CI_Block01_5mall_xyz.txt", Colors::Red, false);
	PointsToParticles("Data/CI_Block02_5mall_xyz.txt", Colors::White, true);
	PointsToParticles("Data/CI_Block03_5mall_xyz.txt", Colors::Orange, false);
	PointsToParticles("Data/CI_Block04_5mall_xyz.txt", Colors::Yellow, true);
	PointsToParticles("Data/CI_Block05_5mall_xyz.txt", Colors::Green, false);
	PointsToParticles("Data/CI_Block06_5mall_xyz.txt", Colors::Cyan, false);
	PointsToParticles("Data/CI_Block07_5mall_xyz.txt", Colors::Blue, false);
	PointsToParticles("Data/CI_Block08_5mall_xyz.txt", Colors::Magenta, true);
	PointsToParticles("Data/CI_Block09_5mall_xyz.txt", color3i(128, 128, 0), true);
	PointsToParticles("Data/CI_Block10_5mall_xyz.txt", color3i(128, 128, 128), true);
	PointsToParticles("Data/CI_Block11_5mall_xyz.txt", color3i(0, 128, 128), true);
	PointsToParticles("Data/CI_Block12_5mall_xyz.txt", color3i(128, 0, 128), false);

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
	ParticleSystem->Update();

	WaterSurface = new CWaterSurfaceSceneObject();
	WaterSurface->SceneColor = SwapColor;
	//WaterRenderPass->AddSceneObject(WaterSurface);

	Volume = new CVolumeSceneObject();
	vec3u VolumeSize = vec3u(14, 23, 28);
	SharedPointer<Graphics::ITexture3D> VolumeData = GraphicsAPI->CreateTexture3D(VolumeSize, ITexture::EMipMaps::False, ITexture::EFormatComponents::RGBA, ITexture::EInternalFormatType::Fix8);
	vector<double> DataNorm = LoadData("Data/oxyMaps.csv");
	byte * Data = new byte[28 * 23 * 14 * 4];
	for (int k = 0; k < 28; ++ k)
	{
		for (int j = 0; j < 23; ++ j)
		{
			for (int i = 0; i < 14; ++ i)
			{
				int const Index = i + j * 14 + k * 23 * 14;
				color4i const Color = NormDataToColor(DataNorm[Index]);

				for (int t = 0; t < 4; ++ t)
				{
					Data[Index * 4 + t] = Color[t];
				}
			}
		}
	}
	VolumeData->Upload(Data, VolumeSize, ITexture::EFormatComponents::RGBA, EScalarType::UnsignedInt8);
	delete[] Data;
	VolumeData->SetMinFilter(ITexture::EFilter::Linear);
	VolumeData->SetMagFilter(ITexture::EFilter::Linear);
	VolumeData->SetWrapMode(ITexture::EWrapMode::Clamp);
	Volume->VolumeData = VolumeData;
	Volume->SceneDepth = SceneDepth;
	Volume->SetScale(vec3f(16.f));
	//Volume->SetPosition(vec3f(1280.f, -1600.f, 1280.f));
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

	//Caustics
	//std::vector<string> CausticFileNames;
	//char bfr[256];
	//for (int i = 0; i < 32; i++)
	//{
	//	sprintf(bfr, "Caustics/save.%02d.png", i);
	//	CausticFileNames.push_back(string(bfr));
	//}
	//SharedPointer<Graphics::ITexture3D> causticTextures = AssetManager->Load3DTexture(CausticFileNames);

	//Shark

	//COrthographicCamera caustCam(64, 64, 64, 64);
	//caustCam.SetPosition(vec3f(20, 20, 5));
	//caustCam.SetLookAtTarget(vec3f(0, 0, 0));
	//caustCam.RecalculateViewMatrix();
	//SharedPointer<ion::Graphics::IUniform> uCaustVPMatrix =
	//	std::make_shared<ion::Graphics::CUniformValue<glm::mat4>>(caustCam.GetProjectionMatrix() * caustCam.GetViewMatrix());

	//SharkObject = new SharkSceneObject("Assets/Models/leopardSharkUnparented.dae");
	//SharkObject->SetUniform("uCaustVPMatrix", uCaustVPMatrix);
	//SharkObject->SetTexture("uCausticTexture", causticTextures);
	//SharkObject->SetShader(Application->SharkShader);
	//SharkObject->TriggerReload();
	//DefaultRenderPass->AddSceneObject(SharkObject);

	//Add texture
	//

	//Spline
	Spline = new KeySpline();
	glm::vec3 offset(0, 0, 0);
	Spline->addNode(SplineNode(offset + glm::vec3(-3, 0, 5)));
	Spline->addNode(SplineNode(offset + glm::vec3(1, 0, 2)));
	Spline->addNode(SplineNode(offset + glm::vec3(2, 0, -4)));
	Spline->addNode(SplineNode(offset + glm::vec3(-3, 0, -8)));
	Spline->addNode(SplineNode(offset + glm::vec3(-3, 0, 5)));
	Spline->addNode(SplineNode(offset + glm::vec3(1, 0, 2)));
	Spline->addNode(SplineNode(offset + glm::vec3(2, 0, -4)));
	Spline->addNode(SplineNode(offset + glm::vec3(-3, 0, -8)));
	Spline->addNode(SplineNode(offset + glm::vec3(-3, 0, 5)));
	Spline->addNode(SplineNode(offset + glm::vec3(1, 0, 2)));
	Spline->addNode(SplineNode(offset + glm::vec3(2, 0, -4)));
	Spline->addNode(SplineNode(offset + glm::vec3(-3, 0, -8)));

	Spline->addNode(SplineNode(glm::vec3(0, 0, -10)));

	Spline2 = new KeySpline();
	glm::vec3 offset2(0, 5, 0);
	Spline2->addNode(SplineNode(offset2 + glm::vec3(-3, 0, -8)));
	Spline2->addNode(SplineNode(offset2 + glm::vec3(-3, 0, 5)));
	Spline2->addNode(SplineNode(offset2 + glm::vec3(1, 0, 2)));
	Spline2->addNode(SplineNode(offset2 + glm::vec3(2, 0, -4)));
	Spline2->addNode(SplineNode(offset2 + glm::vec3(-3, 0, -8)));

}

void CViewerState::Update(float const Elapsed)
{
	SceneFrameBuffer->ClearColorAndDepth();
	CopyFrameBuffer->ClearColorAndDepth();
	totalTime += Elapsed;
	Transform t = Spline2->transformAt(totalTime*0.25f);
	//SharkObject->update(*Spline, 0.033333f);

	//DebugCamera->SetPosition(t.getPosition());
	//DebugCamera->SetLookAtTarget(vec3f(0,0,0));

	DebugCameraControl->Update(Elapsed);

	if (SkyBox)
	{
		SkyBox->SetPosition(DefaultRenderPass->GetActiveCamera()->GetPosition());
	}

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
