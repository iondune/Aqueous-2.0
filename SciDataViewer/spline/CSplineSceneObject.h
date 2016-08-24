
#pragma once

#include "ionScene.h"
#include "CKeySpline.h"


class CSplineSceneObject : public ion::Scene::ISceneObject
{

public:

	CSplineSceneObject(SharedPointer<CKeySpline> spline);

	virtual void Load(ion::Scene::CRenderPass * RenderPass);
	virtual void Draw(ion::Scene::CRenderPass * RenderPass);
	void SetShader(SharedPointer<ion::Graphics::IShaderProgram> shader);

private:

	SharedPointer<CKeySpline> thisSpline;
	SharedPointer<ion::Graphics::IPipelineState> PipelineState;
	SharedPointer<ion::Graphics::IShaderProgram> Shader;

};
