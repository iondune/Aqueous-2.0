
#pragma once

#include <ionEngine.h>


class CViewerState;

class CApplication : public IEventListener, public Singleton<CApplication>
{

public:

	void Run();
	void OnEvent(IEvent & Event);
	void Close();
	
	CWindow * GetWindow();

	SharedPointer<ion::Graphics::IShaderProgram> DiffuseShader;
	SharedPointer<ion::Graphics::IShaderProgram> ParticleShader;

	ion::Scene::CSimpleMesh * CubeMesh = nullptr;

	ion::Graphics::IGraphicsAPI * GraphicsAPI = nullptr;
	SharedPointer<ion::Graphics::IGraphicsContext> Context;
	SharedPointer<ion::Graphics::IRenderTarget> RenderTarget;

private:
	
	void InitWindow();
	void LoadAssets();

	SingletonPointer<CWindowManager> WindowManager;
	SingletonPointer<ion::Scene::CSceneManager> SceneManager;
	SingletonPointer<CTimeManager> TimeManager;
	SingletonPointer<CStateManager> StateManager;
	SingletonPointer<ion::CAssetManager> AssetManager;

	SingletonPointer<CViewerState> ViewerState;


	CWindow * Window = nullptr;

};
