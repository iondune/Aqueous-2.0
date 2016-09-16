
#pragma once

#include <ionEngine.h>


class CParticle
{

public:

	vec3d Data;
	vec3d Position;
	color3f Color;

};

class CParticleSystem : public ion::Scene::ISceneObject
{

public:

	CParticleSystem(SharedPointer<ion::Graphics::IShaderProgram> Shader);

	void Load(ion::Scene::CRenderPass * RenderPass);
	void Draw(ion::Scene::CRenderPass * RenderPass);

	vector<CParticle> Particles;

	void Update();

	void SetParticlesFromData();

	int XField = -1;
	int YField = -1;
	int ZField = -1;

	void NormalizeParticlePositions();
	void UpdateColors();

	ion::Graphics::CUniform<f32> BillboardSize = 0.5f;
	float RampLow = -60, RampHigh = -50;

private:

	int const MaxParticles = 250000;

	vector<f32> InstanceData;
	SingletonPointer<ion::CGraphicsAPI> GraphicsAPI;
	SharedPointer<ion::Graphics::IVertexBuffer> InstanceBuffer;

	SharedPointer<ion::Graphics::IVertexBuffer> VertexBuffer;
	SharedPointer<ion::Graphics::IIndexBuffer> IndexBuffer;

	SharedPointer<ion::Graphics::IPipelineState> PipelineState;
	SharedPointer<ion::Graphics::IShaderProgram> Shader;

};
