
#include "CParticleSystem.h"


void CParticleSystem::Init()
{
	SingletonPointer<CSceneManager> SceneManager;

	PositionBuffer = new ion::GL::VertexBuffer;
	PositionBuffer->Data<f32>(MaxParticles * 3 * sizeof(f32), nullptr, 3);
	ColorBuffer = new ion::GL::VertexBuffer;
	ColorBuffer->Data<f32>(MaxParticles * 3 * sizeof(f32), nullptr, 3);
	
	Node = SceneManager->GetFactory()->AddSceneNode("Particle");

	if (! Node)
	{
		Log::Error("Failed to load particle shader!");
	}
	
	if (Node)
	{
		Node->SetVertexBuffer("vPosition", PositionBuffer);
		Node->SetVertexBuffer("vColor", ColorBuffer);
		Node->SetPrimitiveType(ion::GL::EPrimitiveType::Points);
	}
}

void CParticleSystem::Update()
{
	if ((int) Particles.size() > MaxParticles)
	{
		Particles.erase(Particles.begin(), Particles.end() - MaxParticles);
		assert(Particles.size() == MaxParticles);
	}

	assert(Particles.size() <= MaxParticles);

	Positions.clear();
	Colors.clear();

	size_t const FloatsNeeded = Particles.size() * 3;
	if (Positions.size() < FloatsNeeded)
	{
		Positions.resize(FloatsNeeded);
		Colors.resize(FloatsNeeded);
	}

	for (uint i = 0; i < Particles.size(); ++ i)
	{
		Positions[i * 3 + 0] = Particles[i].Position.X;
		Positions[i * 3 + 1] = Particles[i].Position.Y;
		Positions[i * 3 + 2] = Particles[i].Position.Z;
		Colors[i * 3 + 0] = Particles[i].Color.Red;
		Colors[i * 3 + 1] = Particles[i].Color.Green;
		Colors[i * 3 + 2] = Particles[i].Color.Blue;
	}

	PositionBuffer->SubData(Positions);
	ColorBuffer->SubData(Colors);
}

void CParticleSystem::Draw()
{
	if (Node)
	{
		Node->SetElementCount((uint) Particles.size());
		Node->SetVisible(Particles.size() != 0);
	}
}
