
#pragma once

#include <ionEngine.h>


class CApplication;
class CCameraRig;

class CViewerState : public IState, public Singleton<CViewerState>
{

public:

	void Init();
	void Update(float const Elapsed);
	void GUI();

	void OnEvent(IEvent & Event);

private:
	
	SingletonPointer<CApplication> Application;
	SingletonPointer<CSceneManager> SceneManager;
	SingletonPointer<CImGUIManager> GUIManager;
	
	CCameraController * DebugCameraControl = nullptr;
	CPerspectiveCamera * DebugCamera = nullptr;

	color4f ClearColor;

};
