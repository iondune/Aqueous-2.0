
#include "CWaterSurfaceSceneObject.h"
#include <ionGUI.h>


using namespace ion;

CWaterSurfaceSceneObject::CWaterSurfaceSceneObject()
{
	Shader = AssetManager->LoadShader("WaterSurface");

	uint const GeometrySize = 1024 + 1;

	vector<float> VertexData;
	for (int y = 0; y < GeometrySize; ++ y)
	{
		for (int x = 0; x < GeometrySize; ++ x)
		{
			VertexData.push_back((float) x / 4.f);
			VertexData.push_back((float) y / 4.f);
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

	Speed = 0.21f;

	uTime = 0.f;
	uScale = 10.f;
	uFrequency = 6.356f;
	uHeight = 1.65f;
	uSteepness = 0.5f;

	uSelectWave = -1;
	uNumWaves = 60;
}

void CWaterSurfaceSceneObject::Load(ion::Scene::CRenderPass * RenderPass)
{
	if (! PipelineState)
	{
		PipelineState = RenderPass->GetGraphicsContext()->CreatePipelineState();
		PipelineState->SetProgram(Shader);
		PipelineState->SetIndexBuffer(IndexBuffer);
		PipelineState->SetVertexBuffer(0, VertexBuffer);
		PipelineState->SetUniform("uTime", uTime);
		PipelineState->SetUniform("uScale", uScale);
		PipelineState->SetUniform("uFrequency", uFrequency);
		PipelineState->SetUniform("uHeight", uHeight);
		PipelineState->SetUniform("uSelectWave", uSelectWave);
		PipelineState->SetUniform("uSteepness", uSteepness);
		PipelineState->SetUniform("uNumWaves", uNumWaves);
		PipelineState->SetTexture("uSkyBox", SkyBoxTexture);
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
		ImGui::DragFloat("Steepness", &uSteepness.Get(), 0.01f, 0.f, 1.f);
		ImGui::SliderInt("Wave Count", &uNumWaves.Get(), 1, 110);

		ImGui::Separator();

		ImGui::SliderInt("Wave Debug", &uSelectWave.Get(), 0, uNumWaves.Get());
		if (ImGui::Checkbox("Wireframe?", &Wireframe))
		{
			PipelineState->SetFeatureEnabled(Graphics::EDrawFeature::Wireframe, Wireframe);
		}
	}
	ImGui::End();
}

void CWaterSurfaceSceneObject::ToggleGUI()
{
	ToggleBool(IsGUIVisible);
}
