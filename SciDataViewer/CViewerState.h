
#pragma once

#include <ionEngine.h>


class CApplication;
class CCameraRig;

class CViewerState : public IState, public Singleton<CViewerState>
{

public:

	void Init();
	void Update(float const Elapsed);

	void OnEvent(IEvent & Event);

private:
	
	SingletonPointer<CApplication> Application;
	SingletonPointer<CSceneManager> SceneManager;
	
	CCameraController * DebugCameraControl = nullptr;
	CPerspectiveCamera * DebugCamera = nullptr;

};
