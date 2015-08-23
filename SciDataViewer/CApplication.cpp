
#include "CApplication.h"
#include "CViewerState.h"


void CApplication::Run()
{
	InitWindow();
	LoadAssets();

	ViewerState->Init();
	StateManager->SetState(ViewerState.Get());

	ion::GL::Context::Init();
	ion::GL::Context::SetClearColor(color3f(0.65f, 0.85f, 0.95f));

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
	Window = WindowManager->CreateWindow(vec2i(2560, 1440), "Scientific Data Viewer", EWindowType::Windowed);
	Window->AddChild(this);
}

void CApplication::LoadAssets()
{
	SceneManager->GetShaderLibrary()->SetBaseDirectory("Assets/Shaders");
	SceneManager->GetTextureLibrary()->SetBaseDirectory("Assets/Textures");
	SceneManager->GetMeshLibrary()->SetBaseDirectory("Assets/Meshes");

	SceneManager->GetShaderLibrary()->Load("Diffuse");
	SceneManager->GetShaderLibrary()->Load("Particle");

	SceneManager->GetMeshLibrary()->Add("Cube", CGeometryCreator::CreateCube());
}
