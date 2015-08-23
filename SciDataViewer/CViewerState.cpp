
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
}

void CViewerState::Update(float const Elapsed)
{
	DebugCameraControl->Update(Elapsed);
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
			}
		}
	}
}
