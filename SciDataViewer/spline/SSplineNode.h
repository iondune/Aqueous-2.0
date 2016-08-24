
#pragma once

#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#include <Eigen/Dense>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class SSplineNode
{
public:

	/**
	 * Create a spline node, with a position, and a rotation if
	 * interploation is needed of a model if needed.
	 */
	SSplineNode(const glm::vec3 & position) :
		position(Eigen::Vector3f(position.x, position.y, position.z))
	{}

	SSplineNode(const glm::vec3 & position, const glm::quat & rotation) :
		position(position.x, position.y, position.z),
		rotation(rotation.w, rotation.x, rotation.y, rotation.z)
	{}

	SSplineNode(Eigen::Vector3f position, Eigen::Quaternionf rotation) :
		position(position),
		rotation(rotation)
	{}

	~SSplineNode()
	{}

	const Eigen::Vector3f & GetPosition() const { return position; }
	const Eigen::Quaternionf & GetRotation() const { return rotation; }

private:

	Eigen::Vector3f position;
	Eigen::Quaternionf rotation;

};
