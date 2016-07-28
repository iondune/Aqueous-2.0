#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <ionCore.h>
#include <ionMath.h>
#include <ionScene.h>
#include "VertexBoneData.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Skeleton.h"
/**
 * An Assimp renderable model
 */
class BoneModel
{
public:
	BoneModel(std::string path);
	Bone * const getBone(std::string boneName);
	std::vector<Bone> & getAllBones() { return skeleton.getAllBones(); }
	SharedPointer<ion::Scene::CSimpleMesh> mesh;
	SharedPointer<ion::Graphics::IVertexBuffer> boneData;
	const std::vector<glm::mat4> & getBoneOffsets();
	float getWidth() { return maxBounds.x - minBounds.x; }
	float getHeight() { return maxBounds.y - minBounds.y; }
	float getLength() { return maxBounds.z - minBounds.z; };

	void updateSkeleton();
private:
	void loadModel(std::string path);
	void loadBones(aiNode * node);
	void processNode(aiNode * node, const aiScene * scene);
	glm::vec3 minBounds;
	glm::vec3 maxBounds;

	std::vector<VertexBoneData> processMeshBoneData(aiMesh * mesh);
	SharedPointer<ion::Scene::CSimpleMesh> processMesh(aiMesh * mesh, const aiScene * scene);
	Skeleton skeleton;
	//Animate the model with a loaded animation.
	std::vector<glm::mat4> boneOffsets;
	std::vector<VertexBoneData> globalBoneData;


};

#endif
