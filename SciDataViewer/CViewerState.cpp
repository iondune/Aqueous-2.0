
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

static bool ShowCameraWindow = false;

void CameraWidget(CCameraController * Controller, CPerspectiveCamera * Camera)
{
	vec3f Position = Controller->GetPosition();
	float Phi = Controller->GetPhi();
	float Theta = Controller->GetTheta();
	float NearPlane = Camera->GetNearPlane();
	float FarPlane = Camera->GetFarPlane();
	float FocalLength = Camera->GetFocalLength();
	float Speed = Controller->GetVelocity();

	ImGui::SetNextWindowPos(ImVec2(950, 50), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiSetCond_Once);
	ImGui::Begin("Camera", &ShowCameraWindow);
	ImGui::InputFloat3("Position", &Position.Values[0]);
	ImGui::DragFloat("Phi", &Phi, 0.01f, -1.56f, 1.56f);
	ImGui::DragFloat("Theta", &Theta, 0.01f, -3.1415f, 3.14156f);
	ImGui::InputFloat("Near Plane", &NearPlane);
	ImGui::InputFloat("Far Plane", &FarPlane);
	ImGui::InputFloat("Focal Length", &FocalLength);
	ImGui::InputFloat("Velocity", &Speed);
	ImGui::End();

	Controller->SetPhi(Phi);
	Controller->SetTheta(Theta);
	Controller->SetVelocity(Speed);
	Camera->SetPosition(Position);
	Camera->SetNearPlane(NearPlane);
	Camera->SetFarPlane(FarPlane);
	Camera->SetFocalLength(FocalLength);
}

void CViewerState::GUI()
{
	GUIManager->NewFrame();

	static bool show_test_window = false;
	static bool show_another_window = false;



	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit", "Alt+F4")) { Application->Close();  }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Camera Control")) { ShowCameraWindow ^= 1; };
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}


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

	if (ShowCameraWindow) CameraWidget(DebugCameraControl, DebugCamera);
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

			case EKey::C:
				ShowCameraWindow ^= 1;
				break;
			}
		}
	}
}
