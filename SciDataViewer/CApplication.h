
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

private:
	
	void InitWindow();
	void LoadAssets();

	SingletonPointer<CWindowManager> WindowManager;
	SingletonPointer<CSceneManager> SceneManager;
	SingletonPointer<CTimeManager> TimeManager;
	SingletonPointer<CStateManager> StateManager;

	SingletonPointer<CViewerState> ViewerState;

	CWindow * Window = nullptr;

};
