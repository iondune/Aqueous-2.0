
#include "CVolumeSceneObject.h"
#include <ionGUI.h>


using namespace ion;
using namespace ion::Graphics;
using namespace ion::Scene;

CVolumeSceneObject::CVolumeSceneObject()
	: Mode(2), SliceAxis(vec3f(1.f, 0.f, 0.f)),
	LocalRange(0.2f), MinimumAlpha(0.1f),
	EmphasisLocation(0.5f), AlphaIntensity(1.f),
	StepSize(1.f / 100.f), QualityLevel(10.f), DebugLevel(0), UseShading(0)
{
	Mesh = new CSimpleMesh();

	Mesh->Vertices.resize(24);
	Mesh->Vertices[0].Position = SVector3f(-0.5, -0.5, -0.5);
	Mesh->Vertices[1].Position = SVector3f(-0.5, 0.5, -0.5);
	Mesh->Vertices[2].Position = SVector3f(0.5, 0.5, -0.5);
	Mesh->Vertices[3].Position = SVector3f(0.5, -0.5, -0.5);
	Mesh->Vertices[0].Color = SColorf(0, 0, 0);
	Mesh->Vertices[1].Color = SColorf(0, 1, 0);
	Mesh->Vertices[2].Color = SColorf(1, 1, 0);
	Mesh->Vertices[3].Color = SColorf(1, 0, 0);

	Mesh->Vertices[4].Position = SVector3f(0.5, -0.5, -0.5);
	Mesh->Vertices[5].Position = SVector3f(0.5, 0.5, -0.5);
	Mesh->Vertices[6].Position = SVector3f(0.5, 0.5, 0.5);
	Mesh->Vertices[7].Position = SVector3f(0.5, -0.5, 0.5);
	Mesh->Vertices[4].Color = SColorf(1, 0, 0);
	Mesh->Vertices[5].Color = SColorf(1, 1, 0);
	Mesh->Vertices[6].Color = SColorf(1, 1, 1);
	Mesh->Vertices[7].Color = SColorf(1, 0, 1);

	Mesh->Vertices[8].Position = SVector3f(0.5, -0.5, 0.5);
	Mesh->Vertices[9].Position = SVector3f(0.5, 0.5, 0.5);
	Mesh->Vertices[10].Position = SVector3f(-0.5, 0.5, 0.5);
	Mesh->Vertices[11].Position = SVector3f(-0.5, -0.5, 0.5);
	Mesh->Vertices[8].Color = SColorf(1, 0, 1);
	Mesh->Vertices[9].Color = SColorf(1, 1, 1);
	Mesh->Vertices[10].Color = SColorf(0, 1, 1);
	Mesh->Vertices[11].Color = SColorf(0, 0, 1);

	Mesh->Vertices[12].Position = SVector3f(-0.5, -0.5, 0.5);
	Mesh->Vertices[13].Position = SVector3f(-0.5, 0.5, 0.5);
	Mesh->Vertices[14].Position = SVector3f(-0.5, 0.5, -0.5);
	Mesh->Vertices[15].Position = SVector3f(-0.5, -0.5, -0.5);
	Mesh->Vertices[12].Color = SColorf(0, 0, 1);
	Mesh->Vertices[13].Color = SColorf(0, 1, 1);
	Mesh->Vertices[14].Color = SColorf(0, 1, 0);
	Mesh->Vertices[15].Color = SColorf(0, 0, 0);

	Mesh->Vertices[16].Position = SVector3f(-0.5, 0.5, -0.5);
	Mesh->Vertices[17].Position = SVector3f(-0.5, 0.5, 0.5);
	Mesh->Vertices[18].Position = SVector3f(0.5, 0.5, 0.5);
	Mesh->Vertices[19].Position = SVector3f(0.5, 0.5, -0.5);
	Mesh->Vertices[16].Color = SColorf(0, 1, 0);
	Mesh->Vertices[17].Color = SColorf(0, 1, 1);
	Mesh->Vertices[18].Color = SColorf(1, 1, 1);
	Mesh->Vertices[19].Color = SColorf(1, 1, 0);

	Mesh->Vertices[20].Position = SVector3f(0.5, -0.5, -0.5);
	Mesh->Vertices[21].Position = SVector3f(0.5, -0.5, 0.5);
	Mesh->Vertices[22].Position = SVector3f(-0.5, -0.5, 0.5);
	Mesh->Vertices[23].Position = SVector3f(-0.5, -0.5, -0.5);
	Mesh->Vertices[20].Color = SColorf(1, 0, 0);
	Mesh->Vertices[21].Color = SColorf(1, 0, 1);
	Mesh->Vertices[22].Color = SColorf(0, 0, 1);
	Mesh->Vertices[23].Color = SColorf(0, 0, 0);

	Mesh->Triangles.resize(12);
	for (int i = 0; i < 6; ++ i)
	{
		Mesh->Vertices[4 * i + 0].TextureCoordinates = SVector2f(0, 1);
		Mesh->Vertices[4 * i + 1].TextureCoordinates = SVector2f(0, 0);
		Mesh->Vertices[4 * i + 2].TextureCoordinates = SVector2f(1, 0);
		Mesh->Vertices[4 * i + 3].TextureCoordinates = SVector2f(1, 1);

		Mesh->Triangles[2 * i].Indices[0] = 4 * i + 0;
		Mesh->Triangles[2 * i].Indices[1] = 4 * i + 1;
		Mesh->Triangles[2 * i].Indices[2] = 4 * i + 2;

		Mesh->Triangles[2 * i + 1].Indices[0] = 4 * i + 0;
		Mesh->Triangles[2 * i + 1].Indices[1] = 4 * i + 2;
		Mesh->Triangles[2 * i + 1].Indices[2] = 4 * i + 3;
	}

	Shader = AssetManager->LoadShader("Volume");
}

void CVolumeSceneObject::Load(ion::Scene::CRenderPass * RenderPass)
{
	if (! PipelineState)
	{
		PipelineState = RenderPass->GetGraphicsContext()->CreatePipelineState();
		PipelineState->SetProgram(Shader);
		PipelineState->SetIndexBuffer(Mesh->CreateIndexBuffer());
		PipelineState->SetVertexBuffer(0, Mesh->CreateVertexBuffer());

		PipelineState->SetFeatureEnabled(EDrawFeature::DisableDepthTest, true);
		PipelineState->SetFeatureEnabled(EDrawFeature::CullFront, true);
		PipelineState->SetBlendMode(EBlendMode::Alpha);

		PipelineState->SetUniform("uInvModelMatrix", InvModelMatrix);
		PipelineState->SetUniform("uAlphaIntensity", AlphaIntensity);
		PipelineState->SetUniform("uHighlightMode", Mode);
		PipelineState->SetUniform("uSliceAxis", SliceAxis);
		PipelineState->SetUniform("uLocalRange", LocalRange);
		PipelineState->SetUniform("uMinimumAlpha", MinimumAlpha);
		PipelineState->SetUniform("uEmphasisLocation", EmphasisLocation);
		PipelineState->SetUniform("uStepSize", StepSize);
		PipelineState->SetUniform("uDebugLevel", DebugLevel);
		PipelineState->SetUniform("uUseShading", UseShading);

		PipelineState->SetTexture("uVolumeData", VolumeData);
		PipelineState->SetTexture("uProximityData", ProximityTexture);
		PipelineState->SetTexture("uDepthTexture", SceneDepth);
	}

	RenderPass->PreparePipelineStateForRendering(PipelineState, this);

	Loaded[RenderPass] = true;
}

void CVolumeSceneObject::Draw(ion::Scene::CRenderPass * RenderPass)
{
	InvModelMatrix = glm::inverse(Transformation.Get());
	RenderPass->SubmitPipelineStateForRendering(PipelineState, this, 1, 1);
}

void CVolumeSceneObject::GUI()
{
	if (! IsGUIVisible)
	{
		return;
	}

	ImGui::SetNextWindowPos(ImVec2(700, 50), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiSetCond_Once);
	if (ImGui::Begin("Volume", &IsGUIVisible))
	{
		ImGui::SliderInt("Wave Debug", &DebugLevel.Get(), 0, 6);
		ImGui::SliderInt("Draw Mode", &Mode.Get(), 0, 3);
		ImGui::SliderFloat("Emphasis Location", &EmphasisLocation.Get(), 0.f, 1.f);
		ImGui::SliderFloat("Alpha Intensity", &AlphaIntensity.Get(), 0.5f, 10.f);
		ImGui::SliderFloat("Minimum Alpha", &MinimumAlpha.Get(), 0.f, 0.5f);
		ImGui::SliderFloat("Local Range", &LocalRange.Get(), 0.05f, 0.5f);
		if (ImGui::SliderFloat("Quality", &QualityLevel, 5.f, 200.f, "%.3f", 2.f))
		{
			StepSize = 1 / QualityLevel;
		}
	}
	ImGui::End();
}

void CVolumeSceneObject::ToggleGUI()
{
	ToggleBool(IsGUIVisible);
}
