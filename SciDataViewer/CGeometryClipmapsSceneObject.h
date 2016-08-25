
#pragma once

#include <ionApplication.h>
#include "CToroidalUpdater.h"


//vec3f CartToLatLong(vec3f const & c);

class CGeometryClipmapsSceneObject : public ion::Scene::ISceneObject
{

public:

	class IHeightInput
	{

	public:

		virtual float GetTerrainHeight(vec2i const & Position) = 0;
		virtual color3f GetTerrainColor(vec2i const & Position) = 0;
		virtual vec3f GetTerrainNormal(vec2i const & Position);

	};

	IHeightInput * HeightInput = nullptr;

	static int const GeometrySize = 128;
	static int const HeightmapResolution = GeometrySize + 2;
	static int const LayerCount = 9;

	int DrawLevel = 0;

	CGeometryClipmapsSceneObject();

	virtual void Load(ion::Scene::CRenderPass * RenderPass);
	virtual void Draw(ion::Scene::CRenderPass * RenderPass);

	ion::Graphics::CUniform<int> uDebugDisplay = 0;
	bool UseCameraPosition = false;
	bool UseCameraHeight = false;

	SharedPointer<ion::Graphics::IShaderProgram> Shader;

	class SLayer : public CToroidalUpdater::ISampleUploader
	{

	public:

		// Index data used for rendering - generated once per frame
		vector<unsigned int> IndexData;
		SharedPointer<ion::Graphics::IIndexBuffer> IndexBuffer;

		SharedPointer<ion::Graphics::ITexture2D> HeightMap;
		SharedPointer<ion::Graphics::ITexture2D> ColorMap;
		SharedPointer<ion::Graphics::ITexture2D> NormalMap;

		SharedPointer<ion::Graphics::IPipelineState> PipelineState;

		int Level, ScaleFactor;
		bool Active = true;
		bool Visible = true;

		rect2i ActiveRegion;
		
		// Offset of data "origin" within texture
		vec2i DataOffset;
		CToroidalUpdater Updater;

		// Uniforms sent to shader
		ion::Graphics::CUniform<vec3f> uScale;
		ion::Graphics::CUniform<vec3f> uTranslation;
		ion::Graphics::CUniform<int> uScaleFactor;
		ion::Graphics::CUniform<vec2i> uDataOffset;

		SLayer(CGeometryClipmapsSceneObject * Owner, int const i, ion::Scene::CRenderPass * RenderPass);

		//! Returns the offset in data needed for full update
		vec2i GetDesiredActiveRegion(vec2i const & ViewerPosition) const;
		void SetActiveRegion(vec2i const & ActiveRegion);
		int GenerateAndUploadNewData(vec2i const & DataOffsetMove);

	protected:

		CGeometryClipmapsSceneObject * Owner = nullptr;
		
		int SendSample(int const x1, int const y1, int const x2, int const y2, vec2i const & NewClipPos, vec2i const & NewDataOffset);
		void UploadSample(vec2i const & LowerBound, vec2i const & UpperBound, vec2i const & NewDataOffset);

	};

	void SetWireframeEnabled(bool const Enabled);

	std::vector<SLayer *> Layers;

protected:

	// Shared vertex buffer for each later
	SharedPointer<ion::Graphics::IVertexBuffer> VertexBuffer = nullptr;

	bool Wireframe = false;

	vec2i ActiveCameraPosition;

};
