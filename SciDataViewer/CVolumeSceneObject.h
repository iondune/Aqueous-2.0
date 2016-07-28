
#pragma once

#include <ionApplication.h>


class CVolumeSceneObject : public ion::Scene::ISceneObject
{

public:

	ion::Graphics::CUniform<int> Mode;
	ion::Graphics::CUniform<int> DebugLevel;
	ion::Graphics::CUniform<int> UseShading;
	ion::Graphics::CUniform<vec3f> SliceAxis;
	ion::Graphics::CUniform<float> LocalRange;
	ion::Graphics::CUniform<float> MinimumAlpha;
	ion::Graphics::CUniform<float> EmphasisLocation;
	ion::Graphics::CUniform<float> AlphaIntensity;
	float QualityLevel;
	ion::Graphics::CUniform<float> StepSize;

	SharedPointer<ion::Graphics::ITexture2D> SceneDepth;
	SharedPointer<ion::Graphics::ITexture3D> VolumeData;
	SharedPointer<ion::Graphics::ITexture3D> ProximityTexture;

	CVolumeSceneObject();

	void Load(ion::Scene::CRenderPass * RenderPass);
	void Draw(ion::Scene::CRenderPass * RenderPass);

	void GUI();
	void ToggleGUI();

protected:

	SingletonPointer<ion::CAssetManager> AssetManager;

	ion::Scene::CSimpleMesh * Mesh = nullptr;

	SharedPointer<ion::Graphics::IPipelineState> PipelineState;
	SharedPointer<ion::Graphics::IShaderProgram> Shader;

	ion::Graphics::CUniform<glm::mat4> InvModelMatrix;

	bool IsGUIVisible = false;

};