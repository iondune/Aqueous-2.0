#ifndef __SPLINENODE_H__
#define __SPLINENODE_H__

#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Eigen/Dense>
class SplineNode
{
public:
   /**
    * Create a spline node, with a position, and a rotation if
    * interploation is needed of a model if needed.
    */
    SplineNode(const glm::vec3 & position):
        position(Eigen::Vector3f(position.x, position.y, position.z))

        {

        }
    SplineNode(const glm::vec3 & position, const glm::quat & rotation):
        position(position.x,position.y,position.z),
        rotation(rotation.w,rotation.x,rotation.y,rotation.z)
    {

    }
    SplineNode(Eigen::Vector3f position, Eigen::Quaternionf rotation):
    position(position),
    rotation(rotation){};
    ~SplineNode(){};

    const Eigen::Vector3f & getPosition() const{return position;}
    const Eigen::Quaternionf & getRotation() const{return rotation;}

private:
    Eigen::Vector3f position;
    Eigen::Quaternionf rotation;
};
#endif