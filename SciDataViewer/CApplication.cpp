
#include "CApplication.h"
#include "CViewerState.h"


using namespace ion;
using namespace ion::Graphics;
using namespace ion::Scene;


void CApplication::Run()
{
	InitWindow();
	LoadAssets();

	ViewerState->Init();
	StateManager->SetState(ViewerState.Get());

	TimeManager->Start();
	while (WindowManager->Run())
	{
		TimeManager->Update();

		GUIManager->NewFrame();
		StateManager->DoStateChange();
		if (FixedFrameRate)
		{
			Elapsed = 1 / 30.f;
		}
		else
		{
			Elapsed = (f32) TimeManager->GetElapsedTime();
		}
		StateManager->Update(Elapsed);

		RenderTarget->ClearColorAndDepth();
		SceneManager->DrawAll();

		if (Recording)
		{
			CImage * Image = RenderTarget->ReadImage();

			static int FrameCounter = 0;
			string FileName = String::Build("Frames/%06d.png", FrameCounter);
			FrameCounter ++;

			Image->Write(FileName);
		}

		GUIManager->Draw();
		Window->SwapBuffers();
	}
}

void CApplication::OnEvent(IEvent & Event)
{
	if (InstanceOf<SKeyboardEvent>(Event))
	{
		SKeyboardEvent KeyboardEvent = As<SKeyboardEvent>(Event);

		if (! KeyboardEvent.Pressed)
		{
			switch (KeyboardEvent.Key)
			{

			case EKey::Escape:
				Close();
				break;

			case EKey::F5:
				TimeManager->SetTimeMultiplier(1.0);
				break;

			case EKey::F6:
				TimeManager->SetTimeMultiplier(0.1);
				break;

			case EKey::F7:
				TimeManager->SetTimeMultiplier(0.02);
				break;

			case EKey::F8:
				TimeManager->SetTimeMultiplier(0.0001);
				break;

			}
		}
	}
}

void CApplication::Close()
{
	Window->Close();
}

CWindow * CApplication::GetWindow()
{
	return Window;
}


float CApplication::GetFrameTime() const
{
	return Elapsed;
}

void CApplication::InitWindow()
{
	GraphicsAPI->Init(new Graphics::COpenGLImplementation());
	WindowManager->Init(GraphicsAPI);
	Window = WindowManager->CreateWindow(vec2i(1920, 1080), "[Scientific Data Viewer] Aqueous 2.0", EWindowType::Windowed);
	Window->AddListener(this);
	TimeManager->Init(WindowManager);

	Context = GraphicsAPI->GetWindowContext(Window);
	RenderTarget = Context->GetBackBuffer();

	GUIManager->Init(Window);
	GUIManager->AddFontFromFile("Assets/Fonts/OpenSans.ttf", 20.f);

	this->AddListener(GUIManager);
	GUIManager->AddListener(StateManager);
}

void CApplication::LoadAssets()
{
	SceneManager->Init(GraphicsAPI);
	AssetManager->Init(GraphicsAPI);
	AssetManager->AddAssetPath("Assets/");
	AssetManager->SetShaderPath("Shaders/");
	AssetManager->SetTexturePath("Textures/");

	DiffuseShader = AssetManager->LoadShader("Diffuse");
	ParticleShader = AssetManager->LoadShader("Particle");
	SkyBoxShader = AssetManager->LoadShader("SkyBox");
	QuadCopyShader = AssetManager->LoadShader("QuadCopy");
	SharkShader = AssetManager->LoadShader("DiffuseShark");
	DebugShader = AssetManager->LoadShader("Debug");
	GeometryClipmapsShader = AssetManager->LoadShader("GeometryClipmaps");

	CubeMesh = CGeometryCreator::CreateCube();
}
