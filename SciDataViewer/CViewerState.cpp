
#include "CViewerState.h"
#include "CApplication.h"


void CViewerState::Init()
{
	DebugCamera = SceneManager->GetFactory()->AddPerspectiveCamera(Application->GetWindow()->GetAspectRatio());
	DebugCameraControl = new CGamePadCameraController(DebugCamera);
	DebugCamera->SetPosition(vec3f(-10, 10, 0));
	DebugCameraControl->SetVelocity(10.f);
	DebugCameraControl->SetTheta(0);
	DebugCameraControl->SetPhi(-Constants32::Pi / 4.f);
	DebugCamera->SetFarPlane(50000.f);
	Application->GetWindow()->AddListener(DebugCameraControl);
	SceneManager->GetScene()->SetActiveCamera(DebugCamera);

	SceneManager->GetFactory()->AddMeshNode("Cube", "Diffuse");
	ILightSceneNode * Light = SceneManager->GetFactory()->AddLight(vec3f(-128, 256, 128));
	Light->SetRadius(150.f);

	ClearColor = color3f(0.65f, 0.85f, 0.95f);
	ion::GL::Context::SetClearColor(ClearColor);

	GUIManager->Init(Application->GetWindow(), true);
	GUIManager->AddFontFromFile("Assets/Fonts/OpenSans.ttf", 30.f);
}

void CViewerState::Update(float const Elapsed)
{
	DebugCameraControl->Update(Elapsed);

	GUI();
}

void CViewerState::GUI()
{
	GUIManager->NewFrame();

	static bool show_test_window = false;
	static bool show_another_window = false;


	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiSetCond_Once);
		ImGui::Begin("Main Window", &show_another_window);
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		if (ImGui::ColorEdit4("clear color", (float*)&ClearColor.Values[0], false)) ion::GL::Context::SetClearColor(ClearColor);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 2. Show another simple window, this time using an explicit Begin/End pair
	if (show_another_window)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_Once);
		ImGui::SetNextWindowPos(ImVec2(100, 800), ImGuiSetCond_Once);
		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello");
		ImGui::End();
	}

	// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window)
	{
		ImGui::SetNextWindowPos(ImVec2(950, 20), ImGuiSetCond_Once);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void CViewerState::OnEvent(IEvent & Event)
{
	GUIManager->OnEvent(Event);

	if (InstanceOf<SKeyboardEvent>(Event))
	{
		SKeyboardEvent & KeyboardEvent = As<SKeyboardEvent>(Event);
		
		if (! KeyboardEvent.Pressed)
		{
			switch (KeyboardEvent.Key)
			{
			case EKey::Grave:
				break;
			}
		}
	}
}
