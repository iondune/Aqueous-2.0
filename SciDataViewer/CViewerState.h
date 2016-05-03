
#pragma once

#include <ionEngine.h>


class CApplication;
class CDataManager;
class CParticleSystem;
class CWaterSurfaceSceneObject;

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
	SingletonPointer<ion::CAssetManager> AssetManager;
	SingletonPointer<CDataManager> DataManager;

	ion::Scene::CRenderPass * RenderPass = nullptr;
	ion::Scene::CPerspectiveCamera * DebugCamera = nullptr;
	ion::CCameraController * DebugCameraControl = nullptr;
	CParticleSystem * ParticleSystem = nullptr;
	CWaterSurfaceSceneObject * WaterSurface = nullptr;
	ion::Scene::CSimpleMeshSceneObject * SkyBox = nullptr;

	CCameraWindowWidget * CameraWindow = nullptr;
	CPointsWindowWidget * PointsWindow = nullptr;
	CDebugWindowWidget * DebugWindow = nullptr;

	color4f ClearColor;

	void DoImport();
	void DoExportOBJ();

};
