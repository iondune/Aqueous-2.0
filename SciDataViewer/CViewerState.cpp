
#include "CViewerState.h"
#include "CApplication.h"
#include "CParticleSystem.h"

#include "CCameraWindowWidget.h"
#include "CPointsWindowWidget.h"
#include "CDebugWindowWidget.h"
#include "CGeometryClipmapsWindowWidget.h"

#include "CWaterSurfaceSceneObject.h"
#include "CVolumeSceneObject.h"
#include "CGeometryClipmapsSceneObject.h"
#include "ColorTable.h"
#include "UTMtoLatLong.h"
#include "CBathymetryRasterizer.h"
#include "CatalinaOutline.h"


using namespace ion;
using namespace ion::Graphics;
using namespace ion::Scene;


ion::Scene::CSimpleMesh * CreatePolygonMesh(vector<vec2f> const & Points)
{
	ion::Scene::CSimpleMesh * Mesh = new ion::Scene::CSimpleMesh();

	vector<STriangle2D> const Triangles = TriangulateEarClipping(Points);

	for (size_t i = 0; i < Triangles.size(); i ++)
	{
		uint const Start = (uint) Mesh->Vertices.size();
		vec3f const Normal = vec3f(0, 1, 0);

		Mesh->Vertices.push_back(CSimpleMesh::SVertex(vec3f((Triangles[i].A.X - 33.3f) * 800, 0, (Triangles[i].A.Y + 118.3f) * 800), Normal));
		Mesh->Vertices.push_back(CSimpleMesh::SVertex(vec3f((Triangles[i].B.X - 33.3f) * 800, 0, (Triangles[i].B.Y + 118.3f) * 800), Normal));
		Mesh->Vertices.push_back(CSimpleMesh::SVertex(vec3f((Triangles[i].C.X - 33.3f) * 800, 0, (Triangles[i].C.Y + 118.3f) * 800), Normal));

		CSimpleMesh::STriangle Triangle;
		Triangle.Indices[0] = Start + 0;
		Triangle.Indices[1] = Start + 1;
		Triangle.Indices[2] = Start + 2;
		Mesh->Triangles.push_back(Triangle);
	}

	return Mesh;
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

	// Add cube test object
	//CSimpleMeshSceneObject * Cube = new CSimpleMeshSceneObject();
	//Cube->SetShader(Application->DiffuseShader);
	//Cube->SetMesh(Application->CubeMesh);
	//DefaultRenderPass->AddSceneObject(Cube);

	CPointLight * Light = new CPointLight();
	Light->SetPosition(vec3f(-128, 256, 128));
	Light->SetRadius(150.f);
	DefaultRenderPass->AddLight(Light);

	CDirectionalLight * DirectionLight = new CDirectionalLight();
	DirectionLight->SetPosition(vec3f(-128, 256, 128));
	DirectionLight->SetDirection(vec3f(-3, -20, 4).GetNormalized());
	DefaultRenderPass->AddLight(DirectionLight);

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

	auto PointsToParticles = [this](string const & FileName, color3i const & Color, bool Commas) -> vector<vec3f>
	{
		vector<vec3f> Points = LoadPointsFromXYZTxt(FileName, Commas);

		for (size_t i = 0; i < Points.size(); i += 20)
		{
			vec3f const & Point = Points[i];
			CParticle p;
			p.Color = Color;
			p.Position = vec3f((Point.X - 33.3f) * 800, -Point.Z / 50.0f, (Point.Y + 118.3f) * 800);

			ParticleSystem->Particles.push_back(p);
		}

		return Points;
	};

	size_t ndx = 0;
	size_t Sum = CatalinaOutline.size() - 1;
	for (vec2f const & CatPt : CatalinaOutline)
	{
		CParticle p;
		p.Color = color3f(0.f, 1.f, (float) ndx / (float) Sum);
		p.Position = vec3f((CatPt.X - 33.3f) * 800, 0.0f, (CatPt.Y + 118.3f) * 800);

		Text3D.push_back(make_pair(p.Position, String::Build("%d", ndx)));
		ParticleSystem->Particles.push_back(p);

		ndx ++;
	}

	CStopWatch sw;
	sw.Start();

	//vector<vec3f> RegionPoints = LoadPointsFromESRIASCII("Data/GEBCO2014_-122.3058_30.5922_-115.9466_36.0291_30Sec_ESRIASCII.asc");

	//vector<vec3f> HiResPoints = PointsToParticles("Data/CI_Block02_2m_xyz.txt", Colors::Orange, true);

	//vector<vec3f> CatalinaPoints;
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block01_5mall_xyz.txt", false));
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block02_5mall_xyz.txt", true));
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block03_5mall_xyz.txt", false));
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block04_5mall_xyz.txt", true));
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block05_5mall_xyz.txt", false));
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block06_5mall_xyz.txt", false));
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block07_5mall_xyz.txt", false));
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block08_5mall_xyz.txt", true));
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block09_5mall_xyz.txt", true));
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block10_5mall_xyz.txt", true));
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block11_5mall_xyz.txt", true));
	//AddAtEnd(CatalinaPoints, LoadPointsFromXYZTxt("Data/CI_Block12_5mall_xyz.txt", false));

	Log::Info("Load points from files took %.3f", sw.Stop());

	CBathymetryRasterizer * br_hires = new CBathymetryRasterizer();
	//br_hires->SourceElevationPostings = HiResPoints;
	br_hires->ImageSize = 768;
	br_hires->OutputName = "HiRes.png";
	//br_hires->ConvertAndRasterize(true);
	//br_hires->WriteToFile("HiRes.bin");
	br_hires->ReadFromFile("HiRes.bin");

	CBathymetryRasterizer * br_catalina = new CBathymetryRasterizer();
	//br_catalina->SourceElevationPostings = CatalinaPoints;
	br_catalina->ImageSize = 1024;
	br_catalina->OutputName = "Catlina.png";
	br_catalina->RegionXCorner = 33.15f;
	br_catalina->RegionYCorner = -118.7f;
	br_catalina->RegionXSize = 0.5f;
	br_catalina->RegionYSize = 0.5f;
	//br_catalina->ConvertAndRasterize(true);

	CBathymetryRasterizer * br_region = new CBathymetryRasterizer();
	//br_region->SourceElevationPostings = RegionPoints;
	br_region->ImageSize = 4096;
	br_region->OutputName = "Region.png";
	br_region->RegionXCorner = 29.15f;
	br_region->RegionYCorner = -122.7f;
	br_region->RegionXSize = 8.0f;
	br_region->RegionYSize = 8.0f;
	//br_region->ConvertAndRasterize(false);


	CTopographyRasterizer * tr_catalina = new CTopographyRasterizer();
	tr_catalina->SourceLongLatPostings = CatalinaOutline;
	tr_catalina->ImageSize = 4096;
	tr_catalina->OutputName = "CatalinaTopo.png";
	tr_catalina->RegionXCorner = 33.15f;
	tr_catalina->RegionYCorner = -118.7f;
	tr_catalina->RegionXSize = 0.5f;
	tr_catalina->RegionYSize = 0.5f;
	//tr_catalina->FillInteriorPoints();
	//tr_catalina->RasterizeImage();
	//tr_catalina->WriteToFile("CatalinaTopo.bin");
	tr_catalina->ReadFromFile("CatalinaTopo.bin");

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
	vector<double> DataNorm = LoadOxymapsCSVData("Data/oxyMaps.csv");
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


	class SimpleHeight : public CGeometryClipmapsSceneObject::IHeightInput
	{

	public:

		vector<ITerrainHeightSource *> Layers;

		float LastHeight = 0;
		float Scale = 1;

		float GetTerrainHeight(vec2i const & Position)
		{
			LastHeight = -128;

			vec2f const Pos = vec2f(Position) * Scale / (19200.f * 25.f) + vec2f(33.445f, -118.4865f);

			for (auto Layer : Layers)
			{
				if (Layer->IsPointInBounds(Pos))
				{
					LastHeight = -Layer->GetHeightAtPoint(Pos) * 20.f;
					break;
				}
			}

			return LastHeight;

			//float const Input = (float) (Length(vec2f(Position)));
			//return -cos(Input * 0.01f) * 15 - cos(Input * 0.5f) * 2;
		}

		color3f GetTerrainColor(vec2i const & Position)
		{
			return Colors::White;// LastHeight > 0 ? Colors::Green : Colors::Blue;
		}

	};

	GeometryClipmapsObject = new CGeometryClipmapsSceneObject();
	GeometryClipmapsObject->Shader = Application->GeometryClipmapsShader;
	GeometryClipmapsObject->UseCameraPosition = true;
	float const ClipmapsScale = 20.f;
	SimpleHeight * HeightInput = new SimpleHeight();
	HeightInput->Scale = ClipmapsScale;
	HeightInput->Layers.push_back(tr_catalina);
	HeightInput->Layers.push_back(br_hires);
	//HeightInput->Layers.push_back(br_catalina);
	//HeightInput->Layers.push_back(br_region);
	GeometryClipmapsObject->HeightInput = HeightInput;
	GeometryClipmapsObject->SetScale(vec3f(ClipmapsScale, 1.f, ClipmapsScale));
	DefaultRenderPass->AddSceneObject(GeometryClipmapsObject);


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

	ClipmapsWindow = new CGeometryClipmapsWindowWidget(GeometryClipmapsObject);

	// Caustics
	std::vector<string> CausticFileNames;
	char bfr[256];
	for (int i = 0; i < 32; i++)
	{
		sprintf(bfr, "Caustics/save.%02d.png", i);
		CausticFileNames.push_back(string(bfr));
	}
	//SharedPointer<Graphics::ITexture3D> causticTextures = AssetManager->Load3DTexture(CausticFileNames);

	// Shark
	COrthographicCamera caustCam(64, 64, 64, 64);
	caustCam.SetPosition(vec3f(20, 20, 5));
	caustCam.SetLookAtTarget(vec3f(0, 0, 0));
	caustCam.RecalculateViewMatrix();
	SharedPointer<ion::Graphics::IUniform> uCaustVPMatrix =
		std::make_shared<ion::Graphics::CUniformValue<glm::mat4>>(caustCam.GetProjectionMatrix() * caustCam.GetViewMatrix());

	//SharkObject = new SharkSceneObject("Assets/Models/leopardSharkUnparented.dae");
	//SharkObject->SetUniform("uCaustVPMatrix", uCaustVPMatrix);
	//SharkObject->SetTexture("uCausticTexture", causticTextures);
	//SharkObject->SetShader(Application->SharkShader);
	//SharkObject->TriggerReload();
	//DefaultRenderPass->AddSceneObject(SharkObject);

	//Add texture
	//

	//Spline
	Spline = std::make_shared<CKeySpline>();
	glm::vec3 offset(0, 0, 0);
	Spline->AddNode(SSplineNode(offset + glm::vec3(-3, 0, 5)));
	Spline->AddNode(SSplineNode(offset + glm::vec3(1, 0, 2)));
	Spline->AddNode(SSplineNode(offset + glm::vec3(2, 0, -4)));
	Spline->AddNode(SSplineNode(offset + glm::vec3(-3, 0, -8)));
	Spline->AddNode(SSplineNode(offset + glm::vec3(-3, 0, 5)));
	Spline->AddNode(SSplineNode(offset + glm::vec3(1, 0, 2)));
	Spline->AddNode(SSplineNode(offset + glm::vec3(2, 0, -4)));
	Spline->AddNode(SSplineNode(offset + glm::vec3(-3, 0, -8)));
	Spline->AddNode(SSplineNode(offset + glm::vec3(-3, 0, 5)));
	Spline->AddNode(SSplineNode(offset + glm::vec3(1, 0, 2)));
	Spline->AddNode(SSplineNode(offset + glm::vec3(2, 0, -4)));
	Spline->AddNode(SSplineNode(offset + glm::vec3(-3, 0, -8)));

	Spline->AddNode(SSplineNode(glm::vec3(0, 0, -10)));

	Spline2 = std::make_shared<CKeySpline>();
	glm::vec3 offset2(0, 5, 0);
	Spline2->AddNode(SSplineNode(offset2 + glm::vec3(-3, 0, -8)));
	Spline2->AddNode(SSplineNode(offset2 + glm::vec3(-3, 0, 5)));
	Spline2->AddNode(SSplineNode(offset2 + glm::vec3(1, 0, 2)));
	Spline2->AddNode(SSplineNode(offset2 + glm::vec3(2, 0, -4)));
	Spline2->AddNode(SSplineNode(offset2 + glm::vec3(-3, 0, -8)));

	splineObject = std::make_shared<CSplineSceneObject>(Spline);
	splineObject->SetShader(Application->DebugShader);
	DefaultRenderPass->AddSceneObject(splineObject.get());


	CSimpleMeshSceneObject * SceneObject4 = new CSimpleMeshSceneObject();
	SceneObject4->SetShader(Application->DiffuseShader);
	//SceneObject4->SetFeatureEnabled(EDrawFeature::Wireframe, true);
	SceneObject4->SetMesh(CreatePolygonMesh(CatalinaOutline));
	DefaultRenderPass->AddSceneObject(SceneObject4);

	CSimpleMeshSceneObject * SceneObject5 = new CSimpleMeshSceneObject();
	SceneObject5->SetShader(Application->DiffuseShader);
	//SceneObject4->SetFeatureEnabled(EDrawFeature::Wireframe, true);
	SceneObject5->SetMesh(CreatePolygonMesh(BirdRock));
	DefaultRenderPass->AddSceneObject(SceneObject5);
}

void CViewerState::Update(float const Elapsed)
{
	SceneFrameBuffer->ClearColorAndDepth();
	CopyFrameBuffer->ClearColorAndDepth();
	totalTime += Elapsed;
	Transform t = Spline2->TransformAt(totalTime*0.25f);
	//SharkObject->update(*Spline, Elapsed);

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
			if (ImGui::MenuItem("Clipmaps")) { ClipmapsWindow->ToggleVisibility(); };
			if (ImGui::MenuItem("Debug")) { DebugWindow->ToggleVisibility(); };

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	DebugWindow->DrawIfVisible();
	ClipmapsWindow->DrawIfVisible();
	CameraWindow->DrawIfVisible();
	PointsWindow->DrawIfVisible();
	WaterSurface->GUI();
	Volume->GUI();

	for (auto Pair : Text3D)
	{
		bool InFront;
		vec2i Coords = DebugCamera->GetScreenCoordinates(Pair.first, Application->GetWindow()->GetSize(), &InFront);
		if (InFront)
		{
			GUIManager->Text(Coords, Colors::White, Pair.second.c_str());
		}
	}
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
