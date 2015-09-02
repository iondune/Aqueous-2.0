
#pragma once

#include <ionEngine.h>


class CApplication;
class CDataManager;
class CParticleSystem;

class CCameraWindowWidget;
class CPointsWindowWidget;
class CDebugWindowWidget;

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
	SingletonPointer<CDataManager> DataManager;
	SingletonPointer<CParticleSystem> ParticleSystem;
	
	CCameraController * DebugCameraControl = nullptr;
	CPerspectiveCamera * DebugCamera = nullptr;

	CCameraWindowWidget * CameraWindow = nullptr;
	CPointsWindowWidget * PointsWindow = nullptr;
	CDebugWindowWidget * DebugWindow = nullptr;

	color4f ClearColor;

	void DoImport();
	void DoExportOBJ();

};
