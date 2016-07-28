#ifndef __KEYSPLINE__H__
#define __KEYSPLINE__H__

#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#include <Eigen/Dense>
#include "SplineNode.h"
#include <vector>
#include "glm/glm.hpp"
#include "SplineType.h"
#include "shark/Transform.h"
#include <ionCore.h>
#include <ionGraphics.h>

using namespace std;

class KeySpline
{

public:

	KeySpline();
	~KeySpline();
	void setSplineType(SplineType type);
	void addNode(const SplineNode & node);
	SharedPointer<ion::Graphics::IVertexBuffer> GetVertexBuffer();
	SharedPointer<ion::Graphics::IIndexBuffer>  GetIndexBuffer();

	void close(); //Close the spline.
	float sToU(float s);
	int getNumNodes();

	Transform transformAt(float s) const;
	glm::vec3 normalAt(float s) const;
private:
	vector<pair<float, float> > usTable;
	std::vector<Eigen::Vector3f> nodePos;
	std::vector<Eigen::Quaternionf> nodeRot;

	SharedPointer<ion::Graphics::IVertexBuffer> VertexBuffer;
	SharedPointer<ion::Graphics::IIndexBuffer> IndexBuffer;

	Eigen::Matrix4f B;
	SplineType type;

	bool usTableDirty;

	bool updateVBO = true;
	int numSplines;

	void recalculateTable(int discretization);


};

#endif
