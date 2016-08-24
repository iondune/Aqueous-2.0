
#include "CSplineSceneObject.h"


CSplineSceneObject::CSplineSceneObject(SharedPointer<CKeySpline> spline) :
	thisSpline(spline)
{}

void CSplineSceneObject::Load(ion::Scene::CRenderPass * RenderPass)
{
	if (! Shader)
	{
		return;
	}

	if (! PipelineState)
	{
		PipelineState = RenderPass->GetGraphicsContext()->CreatePipelineState();
	}

	PipelineState->SetProgram(Shader);
	PipelineState->SetVertexBuffer(0, thisSpline->GetVertexBuffer());
	PipelineState->SetIndexBuffer(thisSpline->GetIndexBuffer());
	PipelineState->SetPrimitiveType(ion::Graphics::EPrimitiveType::Line);
	RenderPass->PreparePipelineStateForRendering(PipelineState, this);

	Loaded[RenderPass] = true;
}

void CSplineSceneObject::SetShader(SharedPointer<ion::Graphics::IShaderProgram> Shader)
{
	this->Shader = Shader;
	TriggerReload();
}


void CSplineSceneObject::Draw(ion::Scene::CRenderPass * RenderPass)
{
	if (PipelineState)
	{
		RenderPass->SubmitPipelineStateForRendering(PipelineState, this, 1, 0);
	}
}
