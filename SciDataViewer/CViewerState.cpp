
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

color3i ch20m151010[] =
{
	color3i(0,0,0),
	color3i(0,1,0),
	color3i(0,2,0),
	color3i(1,4,1),
	color3i(1,5,1),
	color3i(1,7,1),
	color3i(2,8,2),
	color3i(3,9,2),
	color3i(3,11,2),
	color3i(4,12,2),
	color3i(5,13,2),
	color3i(6,15,2),
	color3i(6,16,2),
	color3i(7,17,2),
	color3i(8,18,2),
	color3i(9,19,2),
	color3i(11,20,2),
	color3i(12,22,2),
	color3i(13,23,2),
	color3i(14,24,2),
	color3i(16,25,2),
	color3i(17,26,2),
	color3i(19,26,2),
	color3i(20,27,2),
	color3i(22,28,2),
	color3i(24,29,2),
	color3i(25,30,3),
	color3i(27,31,3),
	color3i(29,31,3),
	color3i(31,32,3),
	color3i(33,33,3),
	color3i(35,33,4),
	color3i(37,34,4),
	color3i(39,34,5),
	color3i(41,35,5),
	color3i(43,35,6),
	color3i(45,36,6),
	color3i(48,36,7),
	color3i(50,37,8),
	color3i(52,37,8),
	color3i(54,38,9),
	color3i(57,38,10),
	color3i(59,38,11),
	color3i(61,39,12),
	color3i(64,39,13),
	color3i(66,39,15),
	color3i(68,39,16),
	color3i(70,40,17),
	color3i(73,40,19),
	color3i(75,40,20),
	color3i(77,41,22),
	color3i(80,41,24),
	color3i(82,41,25),
	color3i(84,41,27),
	color3i(86,41,29),
	color3i(88,42,31),
	color3i(90,42,33),
	color3i(93,42,35),
	color3i(95,42,37),
	color3i(97,43,40),
	color3i(98,43,42),
	color3i(100,43,44),
	color3i(102,44,47),
	color3i(104,44,49),
	color3i(106,44,52),
	color3i(107,45,54),
	color3i(109,45,57),
	color3i(111,45,60),
	color3i(112,46,62),
	color3i(114,46,65),
	color3i(115,47,68),
	color3i(116,47,71),
	color3i(117,48,74),
	color3i(118,48,77),
	color3i(120,49,80),
	color3i(121,50,83),
	color3i(121,50,85),
	color3i(122,51,88),
	color3i(123,52,91),
	color3i(124,53,94),
	color3i(124,53,98),
	color3i(125,54,101),
	color3i(125,55,104),
	color3i(126,56,107),
	color3i(126,57,109),
	color3i(126,58,112),
	color3i(126,59,115),
	color3i(126,60,118),
	color3i(126,61,121),
	color3i(126,63,124),
	color3i(126,64,127),
	color3i(126,65,130),
	color3i(126,67,132),
	color3i(125,68,135),
	color3i(125,69,138),
	color3i(124,71,140),
	color3i(124,72,143),
	color3i(123,74,145),
	color3i(123,75,148),
	color3i(122,77,150),
	color3i(121,79,152),
	color3i(120,80,155),
	color3i(120,82,157),
	color3i(119,84,159),
	color3i(118,86,161),
	color3i(117,87,163),
	color3i(116,89,164),
	color3i(115,91,166),
	color3i(114,93,168),
	color3i(113,95,169),
	color3i(112,97,171),
	color3i(111,99,172),
	color3i(110,101,173),
	color3i(109,103,175),
	color3i(107,105,176),
	color3i(106,107,177),
	color3i(105,109,178),
	color3i(104,111,178),
	color3i(103,113,179),
	color3i(102,115,180),
	color3i(101,118,180),
	color3i(100,120,181),
	color3i(99,122,181),
	color3i(98,124,181),
	color3i(97,126,182),
	color3i(96,128,182),
	color3i(96,130,182),
	color3i(95,132,182),
	color3i(94,134,181),
	color3i(93,137,181),
	color3i(93,139,181),
	color3i(92,141,180),
	color3i(92,143,180),
	color3i(91,145,179),
	color3i(91,147,179),
	color3i(91,149,178),
	color3i(90,151,177),
	color3i(90,153,177),
	color3i(90,155,176),
	color3i(90,156,175),
	color3i(90,158,174),
	color3i(90,160,173),
	color3i(90,162,172),
	color3i(91,164,171),
	color3i(91,165,170),
	color3i(91,167,169),
	color3i(92,169,167),
	color3i(92,170,166),
	color3i(93,172,165),
	color3i(94,173,164),
	color3i(95,175,163),
	color3i(96,176,161),
	color3i(97,178,160),
	color3i(98,179,159),
	color3i(99,180,158),
	color3i(100,182,156),
	color3i(102,183,155),
	color3i(103,184,154),
	color3i(104,185,153),
	color3i(106,186,152),
	color3i(108,188,150),
	color3i(109,189,149),
	color3i(111,190,148),
	color3i(113,190,147),
	color3i(115,191,146),
	color3i(117,192,145),
	color3i(119,193,145),
	color3i(121,194,144),
	color3i(123,195,143),
	color3i(126,195,142),
	color3i(128,196,142),
	color3i(130,197,141),
	color3i(133,197,140),
	color3i(135,198,140),
	color3i(137,198,140),
	color3i(140,199,139),
	color3i(143,199,139),
	color3i(145,199,139),
	color3i(148,200,139),
	color3i(150,200,139),
	color3i(153,201,139),
	color3i(156,201,139),
	color3i(158,201,139),
	color3i(161,201,139),
	color3i(164,202,140),
	color3i(166,202,140),
	color3i(169,202,141),
	color3i(172,202,142),
	color3i(174,203,142),
	color3i(177,203,143),
	color3i(180,203,144),
	color3i(182,203,145),
	color3i(185,203,146),
	color3i(188,203,147),
	color3i(190,204,148),
	color3i(193,204,150),
	color3i(195,204,151),
	color3i(198,204,152),
	color3i(200,204,154),
	color3i(203,204,155),
	color3i(205,205,157),
	color3i(207,205,159),
	color3i(210,205,160),
	color3i(212,205,162),
	color3i(214,205,164),
	color3i(216,206,166),
	color3i(218,206,168),
	color3i(220,206,170),
	color3i(222,207,172),
	color3i(224,207,174),
	color3i(226,207,176),
	color3i(228,208,178),
	color3i(229,208,181),
	color3i(231,209,183),
	color3i(232,209,185),
	color3i(234,210,187),
	color3i(235,210,190),
	color3i(237,211,192),
	color3i(238,211,194),
	color3i(239,212,196),
	color3i(240,213,199),
	color3i(241,213,201),
	color3i(243,214,203),
	color3i(243,215,206),
	color3i(244,216,208),
	color3i(245,217,210),
	color3i(246,218,212),
	color3i(247,218,214),
	color3i(247,219,217),
	color3i(248,220,219),
	color3i(249,221,221),
	color3i(249,222,223),
	color3i(250,223,225),
	color3i(250,225,227),
	color3i(250,226,229),
	color3i(251,227,231),
	color3i(251,228,232),
	color3i(251,229,234),
	color3i(252,231,236),
	color3i(252,232,237),
	color3i(252,233,239),
	color3i(252,234,241),
	color3i(252,236,242),
	color3i(253,237,243),
	color3i(253,239,245),
	color3i(253,240,246),
	color3i(253,241,247),
	color3i(253,243,248),
	color3i(253,244,249),
	color3i(253,246,250),
	color3i(254,247,251),
	color3i(254,249,252),
	color3i(254,250,253),
	color3i(254,252,253),
	color3i(254,253,254),
};

color4i NormDataToColor(double const Value)
{
	int const Index = Clamp<int>((int) (Value * ION_ARRAYSIZE(ch20m151010)), 0, ION_ARRAYSIZE(ch20m151010) - 1);
	color3f r = ch20m151010[Index];
	color4f Ret(r[0], r[1], r[2], Value);
	Ret.Alpha = (float) Value;
	return Ret;
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
	std::vector<string> CausticFileNames;
	char bfr[256];
	for (int i = 0; i < 32; i++)
	{
		sprintf(bfr, "Caustics/save.%02d.png", i);
		CausticFileNames.push_back(string(bfr));
	}
	SharedPointer<Graphics::ITexture3D> causticTextures = AssetManager->Load3DTexture(CausticFileNames);
	//Shark

	COrthographicCamera caustCam(64, 64, 64, 64);
	caustCam.SetPosition(vec3f(20, 20, 5));
	caustCam.SetLookAtTarget(vec3f(0, 0, 0));
	caustCam.RecalculateViewMatrix();
	SharedPointer<ion::Graphics::IUniform> uCaustVPMatrix =
		std::make_shared<ion::Graphics::CUniformValue<glm::mat4>>(caustCam.GetProjectionMatrix() * caustCam.GetViewMatrix());

	SharkObject = new SharkSceneObject("Assets/Models/leopardSharkUnparented.dae");
	SharkObject->SetUniform("uCaustVPMatrix", uCaustVPMatrix);
	SharkObject->SetTexture("uCausticTexture", causticTextures);
	SharkObject->SetShader(Application->SharkShader);
	SharkObject->TriggerReload();
	DefaultRenderPass->AddSceneObject(SharkObject);

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
	SharkObject->update(*Spline, 0.033333f);

	//DebugCamera->SetPosition(t.getPosition());
	//DebugCamera->SetLookAtTarget(vec3f(0,0,0));
	DebugCameraControl->Update(Elapsed);

	SkyBox->SetPosition(DefaultRenderPass->GetActiveCamera()->GetPosition());
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
