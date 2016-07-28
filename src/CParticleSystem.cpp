
#include "CParticleSystem.h"
#include "CDataManager.h"

using namespace ion::Graphics;


CParticleSystem::CParticleSystem(SharedPointer<ion::Graphics::IShaderProgram> Shader)
{
	this->Shader = Shader;

	vector<u32> const Indices
	{
		0, 1, 2,
		0, 2, 3,
	};

	IndexBuffer = GraphicsAPI->CreateIndexBuffer();
	IndexBuffer->UploadData(Indices);

	vector<f32> const Vertices
	{
		0.5f,  0.5f,   1, 1,
		0.5f, -0.5f,   1, 0,
		-0.5f, -0.5f,   0, 0,
		-0.5f,  0.5f,   0, 1,
	};
	SInputLayoutElement const InputLayout[] =
	{
		{ "vPosition", 2, EAttributeType::Float },
		{ "vTexCoords", 2, EAttributeType::Float },
	};

	VertexBuffer = GraphicsAPI->CreateVertexBuffer();
	VertexBuffer->UploadData(Vertices);
	VertexBuffer->SetInputLayout(InputLayout, ION_ARRAYSIZE(InputLayout));

	InstanceBuffer = GraphicsAPI->CreateVertexBuffer();
	InstanceBuffer->SetInstancingEnabled(true);
	SInputLayoutElement InstanceLayout[] =
	{
		{ "vInstancePosition", 3, EAttributeType::Float },
		{ "vInstanceColor", 3, EAttributeType::Float },
	};
	InstanceBuffer->SetInputLayout(InstanceLayout, ION_ARRAYSIZE(InstanceLayout));
}

void CParticleSystem::Load(ion::Scene::CRenderPass * RenderPass)
{
	PipelineState = RenderPass->GetGraphicsContext()->CreatePipelineState();
	PipelineState->SetProgram(Shader);
	PipelineState->SetIndexBuffer(IndexBuffer);
	PipelineState->SetVertexBuffer(0, VertexBuffer);
	PipelineState->SetVertexBuffer(1, InstanceBuffer);
	PipelineState->SetUniform("uParticleSize", BillboardSize);
	PipelineState->SetFeatureEnabled(EDrawFeature::DisableDepthWrite, true);

	RenderPass->PreparePipelineStateForRendering(PipelineState, this);
	Loaded[RenderPass] = true;
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



static color3f MapColor(f32 const v)
{
	f32 Color[4] = { 0.f, 0.f, 0.f, 1.f };

	if (v <= 0.0)
	{
	}
	else if (v <= 0.1)
	{
		Color[2] = (f32) ((v - 0.0) / (0.1 - 0.0));
	}
	else if (0.2 >= v && v >= 0.1)
	{
		Color[1] = (f32) ((v - 0.1) / (0.2 - 0.1));
		Color[2] = 1.f;
	}
	else if (0.4 >= v && v >= 0.2)
	{
		Color[1] = 1.f;
		Color[2] = 1.f - (f32) ((v - 0.2) / (0.4 - 0.2));
	}
	else if (0.6 >= v && v >= 0.4)
	{
		Color[0] = (f32) ((v - 0.4) / (0.6 - 0.4));
		Color[1] = 1.f;
	}
	else if (0.8 >= v && v >= 0.6)
	{
		Color[0] = 1.f;
		Color[1] = 1.f - (f32) ((v - 0.6) / (0.8 - 0.6));
	}
	else if (v <= 1.0)
	{
		Color[0] = 1.f;
		Color[1] = Color[2] = (f32) ((v - 0.8) / (1.0 - 0.8));
	}
	else
	{
		Color[0] = Color[1] = Color[2] = 1.f;
	}

	Color[3] = (f32) v;

	return color3f(Color[0], Color[1], Color[2]);// , Color[3]);
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
		Point.Color = MapColor((f32) va);
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
		Point.Color = MapColor((f32) va);
	}
}

void CParticleSystem::Update()
{
	InstanceData.clear();

	size_t const FloatsNeeded = Particles.size() * 6;
	if (InstanceData.size() < FloatsNeeded)
	{
		InstanceData.resize(FloatsNeeded);
	}

	for (uint i = 0; i < Particles.size(); ++ i)
	{
		InstanceData[i * 6 + 0] = (f32) Particles[i].Position.X;
		InstanceData[i * 6 + 1] = (f32) Particles[i].Position.Y;
		InstanceData[i * 6 + 2] = (f32) Particles[i].Position.Z;
		InstanceData[i * 6 + 3] = Particles[i].Color.Red;
		InstanceData[i * 6 + 4] = Particles[i].Color.Green;
		InstanceData[i * 6 + 5] = Particles[i].Color.Blue;
	}

	InstanceBuffer->UploadData(InstanceData);
}

void CParticleSystem::Draw(ion::Scene::CRenderPass * RenderPass)
{
	if (Particles.size())
	{
		//RenderPass->SubmitPipelineStateForRendering(PipelineState, this, (uint) Particles.size() / 6);
	}
}
