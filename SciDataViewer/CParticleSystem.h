
#pragma once

#include <ionEngine.h>


class CParticle
{

public:

	vec3d Data;
	vec3d Position;
	color3f Color;

};

class CParticleSystem : public Singleton<CParticleSystem>
{

public:

	void Init();

	vector<CParticle> Particles;

	void Update();
	void Draw();

	void SetParticlesFromData();

	int XField = -1;
	int YField = -1;
	int ZField = -1;

	void NormalizeParticlePositions();
	void UpdateColors();

	CUniformValue<f32> BillboardSize = 0.08f;
	float RampLow = -60, RampHigh = -50;

private:

	int const MaxParticles = 250000;

	CSceneNode * Node;
	
	vector<f32> Positions;
	vector<f32> Colors;
	ion::GL::VertexBuffer * PositionBuffer = nullptr;
	ion::GL::VertexBuffer * ColorBuffer = nullptr;


	friend Singleton<CParticleSystem>;
	CParticleSystem() {}

};
