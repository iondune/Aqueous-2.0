
#pragma once

#include <ionEngine.h>


class CViewerState;

class CApplication : public IEventListener, public Singleton<CApplication>
{

public:

	void Run();
	void OnEvent(IEvent & Event);
	void Close();

	ion::CWindow * GetWindow();

	SharedPointer<ion::Graphics::IShaderProgram> DiffuseShader;
	SharedPointer<ion::Graphics::IShaderProgram> ParticleShader;
	SharedPointer<ion::Graphics::IShaderProgram> SkyBoxShader;
	SharedPointer<ion::Graphics::IShaderProgram> QuadCopyShader;
	SharedPointer<ion::Graphics::IShaderProgram> SharkShader;
	SharedPointer<ion::Graphics::IShaderProgram> DebugShader;
	SharedPointer<ion::Graphics::IShaderProgram> GeometryClipmapsShader;

	ion::Scene::CSimpleMesh * CubeMesh = nullptr;

	SharedPointer<ion::Graphics::IGraphicsContext> Context;
	SharedPointer<ion::Graphics::IRenderTarget> RenderTarget;

	float GetFrameTime() const;

	bool Recording = false;
	bool FixedFrameRate = false;

private:

	void InitWindow();
	void LoadAssets();

	SingletonPointer<ion::CGraphicsAPI> GraphicsAPI;
	SingletonPointer<ion::CWindowManager> WindowManager;
	SingletonPointer<ion::CSceneManager> SceneManager;
	SingletonPointer<ion::CTimeManager> TimeManager;
	SingletonPointer<ion::CStateManager> StateManager;
	SingletonPointer<ion::CAssetManager> AssetManager;
	SingletonPointer<ion::CGUIManager> GUIManager;

	SingletonPointer<CViewerState> ViewerState;


	ion::CWindow * Window = nullptr;

	float Elapsed = 0;

};
