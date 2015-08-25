
#include "CViewerState.h"
#include "CApplication.h"
#include "CParticleSystem.h"


void CViewerState::Init()
{
	DebugCamera = SceneManager->GetFactory()->AddPerspectiveCamera(Application->GetWindow()->GetAspectRatio());
	DebugCameraControl = new CGamePadCameraController(DebugCamera);
	DebugCamera->SetPosition(vec3f(-10, 10, 0));
	DebugCameraControl->SetVelocity(10.f);
	DebugCameraControl->SetTheta(0);
	DebugCameraControl->SetPhi(-Constants32::Pi / 4.f);
	DebugCamera->SetFarPlane(50000.f);
	AddListener(DebugCameraControl);
	SceneManager->GetScene()->SetActiveCamera(DebugCamera);

	SceneManager->GetFactory()->AddMeshNode("Cube", "Diffuse");
	ILightSceneNode * Light = SceneManager->GetFactory()->AddLight(vec3f(-128, 256, 128));
	Light->SetRadius(150.f);

	ParticleSystem->Init();

	for (int x = 1; x < 20; ++ x)
	for (int y = 1; y < 20; ++ y)
	for (int z = 1; z < 20; ++ z)
	{
		static int const NumColors = 7;
		static color3f const ColorChoices[] = {Colors::Red, Colors::Green, Colors::Blue, Colors::Cyan, Colors::Magenta, Colors::Yellow, Colors::Orange};
		CParticle p;
		p.Color = ColorChoices[rand() % NumColors];
		p.Position = vec3f((f32) x, (f32) y, (f32) z);

		ParticleSystem->Particles.push_back(p);
	}
	ParticleSystem->Update();

	ClearColor = color3f(0.65f, 0.85f, 0.95f);
	ion::GL::Context::SetClearColor(ClearColor);

	GUIManager->Init(Application->GetWindow(), true);
	GUIManager->AddFontFromFile("Assets/Fonts/OpenSans.ttf", 30.f);
}

void CViewerState::Update(float const Elapsed)
{
	DebugCameraControl->Update(Elapsed);
	ParticleSystem->Draw();

	GUI();
}

static bool ShowCameraWindow = false;
static bool ShowPointsWindow = false;

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

void PointsWidget()
{
	SingletonPointer<CParticleSystem> ParticleSystem;
	ImGui::SetNextWindowPos(ImVec2(950, 50), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiSetCond_Once);
	ImGui::Begin("Points", &ShowPointsWindow);

	ImGui::Columns(3, "mycolumns");
	ImGui::Separator();
	ImGui::Text("#"); ImGui::NextColumn();
	ImGui::Text("Position"); ImGui::NextColumn();
	ImGui::Text("Color"); ImGui::NextColumn();
	ImGui::Separator();

	for (size_t i = 0; i < ParticleSystem->Particles.size(); i++)
	{
		CParticle const & p = ParticleSystem->Particles[i];

		ImGui::Text("%d", i); ImGui::NextColumn();
		ImGui::Text("%.2f, %.2f, %.2f", p.Position.X, p.Position.Y, p.Position.Z); ImGui::NextColumn();
		ImGui::Text("%.2f, %.2f, %.2f", p.Color.Red, p.Color.Green, p.Color.Blue); ImGui::NextColumn();
	}
	ImGui::Columns(1);

	ImGui::End();
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
		if (ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("Camera")) { ShowCameraWindow ^= 1; };
			if (ImGui::MenuItem("Points")) { ShowPointsWindow ^= 1; };
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
	if (ShowPointsWindow) PointsWidget();
}

void CViewerState::OnEvent(IEvent & Event)
{
	GUIManager->OnEvent(Event);

	if (GUIManager->IO.WantCaptureMouse)
	{
		Event.Block();
	}

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

			case EKey::P:
				ShowPointsWindow ^= 1;
				break;
			}
		}
	}
}
