
#pragma once

#include <ionEngine.h>


class CParticle
{

public:

	vec3f Position;
	color3f Color;

};

class CParticleSystem : public Singleton<CParticleSystem>
{

public:

	void Init();

	vector<CParticle> Particles;

	void Update();
	void Draw();

private:

	int const MaxParticles = 5000;

	CSceneNode * Node;
	
	vector<f32> Positions;
	vector<f32> Colors;
	ion::GL::VertexBuffer * PositionBuffer = nullptr;
	ion::GL::VertexBuffer * ColorBuffer = nullptr;

	friend Singleton<CParticleSystem>;
	CParticleSystem() {}

};
