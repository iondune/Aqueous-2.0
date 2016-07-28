#pragma once
#include "ionScene.h"
#include "KeySpline.h"

class SplineSceneObject : public ion::Scene::ISceneObject
{
public:
	SplineSceneObject(SharedPointer<KeySpline> spline);

	virtual void Load(ion::Scene::CRenderPass * RenderPass);
	virtual void Draw(ion::Scene::CRenderPass * RenderPass);
	void SetShader(SharedPointer<ion::Graphics::IShaderProgram> shader);
private:
	SharedPointer<KeySpline> thisSpline;
	SharedPointer<ion::Graphics::IPipelineState> PipelineState;
	SharedPointer<ion::Graphics::IShaderProgram> Shader;


};
