
#include "CWaterSurfaceSceneObject.h"
#include <ionGUI.h>


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

	Speed = 0.1f;

	uTime = 0.f;
	uScale = 1.f;
	uFrequency = 6.356f;
	uHeight = 2.765f;
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
		PipelineState->SetUniform("uTime", uTime);
		PipelineState->SetUniform("uScale", uScale);
		PipelineState->SetUniform("uFrequency", uFrequency);
		PipelineState->SetUniform("uHeight", uHeight);
		PipelineState->SetFeatureEnabled(Graphics::EDrawFeature::Wireframe, true);
	}

	RenderPass->PreparePipelineStateForRendering(PipelineState, this);

	Loaded[RenderPass] = true;
}

void CWaterSurfaceSceneObject::Draw(ion::Scene::CRenderPass * RenderPass)
{
	uTime = uTime.Get() + (float) TimeManager->GetElapsedTime() * Speed;
	RenderPass->SubmitPipelineStateForRendering(PipelineState, this);
}

void CWaterSurfaceSceneObject::GUI()
{
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiSetCond_Once);
	if (ImGui::Begin("Water Surface", &IsGUIVisible))
	{
		ImGui::DragFloat("Speed", &Speed, 0.001f, 0.f, 1.f);
		ImGui::DragFloat("Scale", &uScale.Get(), 0.1f, 1.f, 20.f);
		ImGui::DragFloat("Frequency", &uFrequency.Get(), 0.1f, 1.f, 20.f);
		ImGui::DragFloat("Height", &uHeight.Get(), 0.1f, 1.f, 20.f);
		ImGui::End();
	}
}

void CWaterSurfaceSceneObject::ToggleGUI()
{
	ToggleBool(IsGUIVisible);
}
