
#pragma once

#include <ionScene.h>
#include <ionApplication.h>


class CWaterSurfaceSceneObject : public ion::Scene::ISceneObject
{

public:

	CWaterSurfaceSceneObject();

	void Load(ion::Scene::CRenderPass * RenderPass);
	void Draw(ion::Scene::CRenderPass * RenderPass);

protected:

	SingletonPointer<ion::CGraphicsAPI> GraphicsAPI;
	SingletonPointer<ion::CAssetManager> AssetManager;
	SingletonPointer<ion::CTimeManager> TimeManager;

	SharedPointer<ion::Graphics::IShaderProgram> Shader;
	SharedPointer<ion::Graphics::IVertexBuffer> VertexBuffer;
	SharedPointer<ion::Graphics::IIndexBuffer> IndexBuffer;

	SharedPointer<ion::Graphics::IPipelineState> PipelineState;

	ion::Graphics::CUniform<float> uTime;

};
