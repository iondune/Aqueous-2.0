#include "SplineSceneObject.h"

SplineSceneObject::SplineSceneObject(SharedPointer<KeySpline> spline):
thisSpline(spline)
{

}

void SplineSceneObject::Load(ion::Scene::CRenderPass * RenderPass)
{
   if(!Shader)
   {
      return;
   }
   if(!PipelineState)
   {
      PipelineState = RenderPass->GetGraphicsContext()->CreatePipelineState();
   }
   PipelineState->SetProgram(Shader);
   PipelineState->SetVertexBuffer(0,thisSpline->GetVertexBuffer());
   PipelineState->SetIndexBuffer(thisSpline->GetIndexBuffer());
   PipelineState->SetPrimitiveType(ion::Graphics::EPrimitiveType::Line);
   RenderPass->PreparePipelineStateForRendering(PipelineState, this);
   Loaded[RenderPass] = true;
}

void SplineSceneObject::SetShader(SharedPointer<ion::Graphics::IShaderProgram> Shader)
{
   this->Shader = Shader;
   TriggerReload();
}


void SplineSceneObject::Draw(ion::Scene::CRenderPass * RenderPass)
{
   if (PipelineState)
   {
      RenderPass->SubmitPipelineStateForRendering(PipelineState, this, 1, 0);
   }
}