
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
	SingletonPointer<ion::CSceneManager> SceneManager;
	SingletonPointer<CDataManager> DataManager;
	
	ion::CCameraController * DebugCameraControl = nullptr;
	ion::Scene::CPerspectiveCamera * DebugCamera = nullptr;
	CParticleSystem * ParticleSystem = nullptr;

	CCameraWindowWidget * CameraWindow = nullptr;
	CPointsWindowWidget * PointsWindow = nullptr;
	CDebugWindowWidget * DebugWindow = nullptr;

	color4f ClearColor;

	void DoImport();
	void DoExportOBJ();

};
