
#pragma once

#include <ionEngine.h>
#include "shark/SharkSceneObject.h"
#include "spline/CKeySpline.h"
#include "spline/CSplineSceneObject.h"

class CApplication;
class CDataManager;
class CParticleSystem;
class CWaterSurfaceSceneObject;
class CVolumeSceneObject;
class CGeometryClipmapsSceneObject;

class CCameraWindowWidget;
class CPointsWindowWidget;
class CDebugWindowWidget;
class CGeometryClipmapsWindowWidget;


vec3d WorldToLongLat(vec3f const & World);
vec3f LongLatToWorld(vec3d const & LongLat);


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
	SingletonPointer<ion::CGUIManager> GUIManager;
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
	CGeometryClipmapsSceneObject * GeometryClipmapsObject = nullptr;
	ion::Scene::CSimpleMeshSceneObject * SkyBox = nullptr;

	SharkSceneObject * SharkObject = nullptr;
	SharedPointer<CSplineSceneObject> splineObject = nullptr;
	SharedPointer<CKeySpline> Spline, Spline2;
	float totalTime;

	vector<pair<vec3f, string>> Text3D;

	CCameraWindowWidget * CameraWindow = nullptr;
	CPointsWindowWidget * PointsWindow = nullptr;
	CDebugWindowWidget * DebugWindow = nullptr;
	CGeometryClipmapsWindowWidget * ClipmapsWindow = nullptr;

	color4f ClearColor;

	void DoImport();
	void DoExportOBJ();

	struct SAnimationFrame
	{
		vec3f Position;
		vec2f Direction;
	};

	bool AnimationPlayingBack = false;
	float CurrentAnimationTime = 0;
	float const TimePerFrame = 1.f;

	vector<SAnimationFrame> AnimationFrames;

	void WriteFramesToFile();
	void ReadFramesFromFile();

	void ApplyCurrentTime(float const t);
	void PushAnimationFrame();

	void StartAnimationPlayback();
	void StopAnimationPlayback();

};
