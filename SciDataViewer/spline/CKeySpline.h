
#pragma once

#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#include <Eigen/Dense>

#include <ionCore.h>
#include <ionGraphics.h>

#include <glm/glm.hpp>
#include "shark/Transform.h"

#include "SSplineNode.h"


enum class ESplineType
{
	Catmull,
	BSpline
};

class CKeySpline
{

public:

	CKeySpline();
	~CKeySpline();

	void SetSplineType(ESplineType const type);
	void AddNode(SSplineNode const & node);
	SharedPointer<ion::Graphics::IVertexBuffer> GetVertexBuffer();
	SharedPointer<ion::Graphics::IIndexBuffer>  GetIndexBuffer();

	void Close();
	float sToU(float const s);
	int GetNumNodes();

	Transform TransformAt(float const s) const;
	glm::vec3 NormalAt(float const s) const;

private:

	vector<pair<float, float>> usTable;
	std::vector<Eigen::Vector3f> nodePos;
	std::vector<Eigen::Quaternionf> nodeRot;

	SharedPointer<ion::Graphics::IVertexBuffer> VertexBuffer;
	SharedPointer<ion::Graphics::IIndexBuffer> IndexBuffer;

	Eigen::Matrix4f B;
	ESplineType type;

	bool usTableDirty;

	bool updateVBO = true;
	int numSplines;

	void RecalculateTable(int const discretization);

};
