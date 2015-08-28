
#include "CParticleSystem.h"
#include "CDataManager.h"


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
		Node->SetUniform("BillboardSize", &BillboardSize);
		Node->SetPrimitiveType(ion::GL::EPrimitiveType::Points);
	}
}

void CParticleSystem::SetParticlesFromData()
{
	SingletonPointer<CDataManager> DataManager;
	CDataSet * DataSet = DataManager->GetCurrentDataSet();

	if (! DataSet)
	{
		return;
	}

	Particles.resize(DataSet->Size());

	for (uint i = 0; i < DataSet->Size(); ++ i)
	{
		CDataRow const & Row = DataSet->GetRow(i);
		double X = 0, Y = 0, Z = 0;

		if (XField >= 0)
		{
			X = Row.GetFieldAsDouble(XField);
		}
		if (YField >= 0)
		{
			Y = Row.GetFieldAsDouble(YField);
		}
		if (ZField >= 0)
		{
			Z = Row.GetFieldAsDouble(ZField);
		}

		Particles[i].Data = vec3d(X, Y, Z);
		Particles[i].Position = vec3d(X, Y, Z);
	}

	NormalizeParticlePositions();
	UpdateColors();
	Update();
}



static color4f MapColor(f32 const v)
{
	f32 Color[4] = { 0.f, 0.f, 0.f, 1.f };

	if (v <= 0.0)
	{
	}
	else if (v <= 0.1)
	{
		Color[2] = (f32)((v - 0.0) / (0.1 - 0.0));
	}
	else if (0.2 >= v && v >= 0.1)
	{
		Color[1] = (f32)((v - 0.1) / (0.2 - 0.1));
		Color[2] = 1.f;
	}
	else if (0.4 >= v && v >= 0.2)
	{
		Color[1] = 1.f;
		Color[2] = 1.f - (f32)((v - 0.2) / (0.4 - 0.2));
	}
	else if (0.6 >= v && v >= 0.4)
	{
		Color[0] = (f32)((v - 0.4) / (0.6 - 0.4));
		Color[1] = 1.f;
	}
	else if (0.8 >= v && v >= 0.6)
	{
		Color[0] = 1.f;
		Color[1] = 1.f - (f32)((v - 0.6) / (0.8 - 0.6));
	}
	else if (v <= 1.0)
	{
		Color[0] = 1.f;
		Color[1] = Color[2] = (f32)((v - 0.8) / (1.0 - 0.8));
	}
	else
	{
		Color[0] = Color[1] = Color[2] = 1.f;
	}

	Color[3] = (f32)v;

	return SColorAf(Color[0], Color[1], Color[2], Color[3]);
}

void CParticleSystem::NormalizeParticlePositions()
{
	double minx, miny, minz, maxx, maxy, maxz;
	minx = miny = minz = NumericLimits<double>::max();
	maxx = maxy = maxz = -NumericLimits<double>::max();

	for (auto Point : Particles)
	{
		if (Point.Position.X < minx)
		{
			minx = Point.Position.X;
		}
		if (Point.Position.Y < miny)
		{
			miny = Point.Position.Y;
		}
		if (Point.Position.Z < minz)
		{
			minz = Point.Position.Z;
		}

		if (Point.Position.X > maxx)
		{
			maxx = Point.Position.X;
		}
		if (Point.Position.Y > maxy)
		{
			maxy = Point.Position.Y;
		}
		if (Point.Position.Z > maxz)
		{
			maxz = Point.Position.Z;
		}
	}

	vec3d const Extent(maxx - minx, maxy - miny, maxz - minz);

	cout << "Extent: " << Extent << endl;

	f64 colorlow = RampLow;
	f64 colorhigh = RampHigh;

	for (auto & Point : Particles)
	{
		double va = (Point.Position.Y - colorlow) / (colorhigh - colorlow);
		Point.Color = MapColor((f32)va);
	}

	static double const CutoffSize = 100;
	if (Extent.X > CutoffSize || Extent.Y > CutoffSize || Extent.Z > CutoffSize)
	{
		double const Largest = Maximum(Extent.X, Extent.Y, Extent.Z);
		double const Mult = 50 / Largest;

		for (auto & Point : Particles)
		{
			Point.Position = (Point.Position - vec3d(minx, miny, minz)) * Mult;
		}

		printf("Normalized by %f\n", Mult);
	}
}

void CParticleSystem::UpdateColors()
{
	f64 colorlow = RampLow;
	f64 colorhigh = RampHigh;

	for (auto & Point : Particles)
	{
		double va = (Point.Data.Y - colorlow) / (colorhigh - colorlow);
		Point.Color = MapColor((f32)va);
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
		Positions[i * 3 + 0] = (f32) Particles[i].Position.X;
		Positions[i * 3 + 1] = (f32) Particles[i].Position.Y;
		Positions[i * 3 + 2] = (f32) Particles[i].Position.Z;
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
