
#include "CWaterSurfaceSceneObject.h"


using namespace ion;

CWaterSurfaceSceneObject::CWaterSurfaceSceneObject()
{
	Shader = AssetManager->LoadShader("WaterSurface");

	uint const GeometrySize = 128 + 1;

	vector<float> VertexData;
	for (int y = 0; y < GeometrySize; ++ y)
	{
		for (int x = 0; x < GeometrySize; ++ x)
		{
			VertexData.push_back((float) x);
			VertexData.push_back((float) y);
		}
	}
	VertexBuffer = GraphicsAPI->CreateVertexBuffer();
	VertexBuffer->UploadData(VertexData);
	Graphics::SInputLayoutElement InputLayout[] =
	{
		{ "vPosition", 2, Graphics::EAttributeType::Float },
	};
	VertexBuffer->SetInputLayout(InputLayout, ION_ARRAYSIZE(InputLayout));

	vector<uint> IndexData;
	for (int y = 1; y < GeometrySize; ++ y)
	{
		for (int x = 1; x < GeometrySize; ++ x)
		{
			IndexData.push_back((x - 1) + (y - 1) * GeometrySize);
			IndexData.push_back((x - 0) + (y - 1) * GeometrySize);
			IndexData.push_back((x - 0) + (y - 0) * GeometrySize);

			IndexData.push_back((x - 1) + (y - 1) * GeometrySize);
			IndexData.push_back((x - 0) + (y - 0) * GeometrySize);
			IndexData.push_back((x - 1) + (y - 0) * GeometrySize);
		}
	}
	IndexBuffer = GraphicsAPI->CreateIndexBuffer();
	IndexBuffer->UploadData(IndexData);
}

void CWaterSurfaceSceneObject::Load(ion::Scene::CRenderPass * RenderPass)
{
	if (! PipelineState)
	{
		PipelineState = RenderPass->GetGraphicsContext()->CreatePipelineState();
		PipelineState->SetProgram(Shader);
		PipelineState->SetIndexBuffer(IndexBuffer);
		PipelineState->SetIndexBuffer(IndexBuffer);
		PipelineState->SetVertexBuffer(0, VertexBuffer);
		//PipelineState->SetUniform("uTime", uTime);
		PipelineState->SetFeatureEnabled(Graphics::EDrawFeature::Wireframe, true);
	}

	RenderPass->PreparePipelineStateForRendering(PipelineState, this);

	Loaded[RenderPass] = true;
}

void CWaterSurfaceSceneObject::Draw(ion::Scene::CRenderPass * RenderPass)
{
	//uTime = uTime.Get() + (float) TimeManager->GetElapsedTime();
	RenderPass->SubmitPipelineStateForRendering(PipelineState, this);
}
