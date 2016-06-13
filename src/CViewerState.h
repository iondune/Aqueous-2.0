
#pragma once

#include <ionEngine.h>
#include "shark/SharkSceneObject.h"
#include "spline/KeySpline.h"

class CApplication;
class CDataManager;
class CParticleSystem;
class CWaterSurfaceSceneObject;
class CVolumeSceneObject;

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
	SingletonPointer<CDataManager> DataManager;

	SingletonPointer<ion::CSceneManager> SceneManager;
	SingletonPointer<ion::CAssetManager> AssetManager;
	SingletonPointer<ion::CGraphicsAPI> GraphicsAPI;

	ion::Scene::CRenderPass * DefaultRenderPass = nullptr;
	ion::Scene::CRenderPass * VolumeRenderPass = nullptr;
	ion::Scene::CRenderPass * CopyRenderPass = nullptr;
	ion::Scene::CRenderPass * WaterRenderPass = nullptr;
	ion::Scene::CRenderPass * FinalRenderPass = nullptr;

	SharedPointer<ion::Graphics::IFrameBuffer> SceneFrameBuffer;
	SharedPointer<ion::Graphics::IFrameBuffer> CopyFrameBuffer;
	SharedPointer<ion::Graphics::ITexture2D> SceneColor;
	SharedPointer<ion::Graphics::ITexture2D> SwapColor;
	SharedPointer<ion::Graphics::ITexture2D> SceneDepth;

	ion::Scene::CPerspectiveCamera * DebugCamera = nullptr;
	ion::CCameraController * DebugCameraControl = nullptr;
	CParticleSystem * ParticleSystem = nullptr;
	CWaterSurfaceSceneObject * WaterSurface = nullptr;
	CVolumeSceneObject * Volume = nullptr;
	ion::Scene::CSimpleMeshSceneObject * SkyBox = nullptr;

	SharkSceneObject * SharkObject = nullptr;
	KeySpline * Spline, *Spline2;
	float totalTime;


	CCameraWindowWidget * CameraWindow = nullptr;
	CPointsWindowWidget * PointsWindow = nullptr;
	CDebugWindowWidget * DebugWindow = nullptr;

	color4f ClearColor;

	void DoImport();
	void DoExportOBJ();

};
