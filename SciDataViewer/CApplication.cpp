
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

	TimeManager->Init();
	while (! WindowManager->ShouldClose())
	{
		TimeManager->Update();
		WindowManager->PollEvents();

		StateManager->DoStateChange();
		StateManager->Update((f32) TimeManager->GetElapsedTime());

		SceneManager->DrawAll();
		ImGui::Render();
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


void CApplication::InitWindow()
{
	WindowManager->Init();
	Window = WindowManager->CreateWindow(vec2i(2560, 1440), "[Scientific Data Viewer] Aqueous 2.0", EWindowType::Windowed);
	Window->AddListener(this);

	GraphicsAPI = new COpenGLAPI();
	Context = GraphicsAPI->GetWindowContext(Window);
	RenderTarget = Context->GetBackBuffer();
}

void CApplication::LoadAssets()
{
	SceneManager->Init(GraphicsAPI);
	AssetManager->Init(GraphicsAPI);
	AssetManager->SetAssetPath("Assets/");
	AssetManager->SetShaderPath("Shaders/");
	AssetManager->SetTexturePath("Textures/");

	DiffuseShader = AssetManager->LoadShader("Diffuse");
	ParticleShader = AssetManager->LoadShader("Particle");

	CubeMesh = CGeometryCreator::CreateCube();
}
