#include "Model.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
BoneModel::BoneModel(std::string path)
{
	loadModel(path);
}


void BoneModel::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_LimitBoneWeights);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Could not load scene at " << path << "," << importer.GetErrorString();
		return;
	}
	if (scene)
	{
		//Load all bones first.
		this->loadBones(scene->mRootNode);

		this->processNode(scene->mRootNode->mChildren[0], scene);
		for (uint i = 0; i < scene->mRootNode->mNumChildren; i++)
		{
			this->processNode(scene->mRootNode->mChildren[i], scene);
		}
		boneData = VertexBoneData::CreateVertexBuffer(globalBoneData);

		//Set the binding matricies of the skeleton
		skeleton.finalize();
		skeleton.finalizeAnimation();


	}



	else ("Could not load mesh at " + path);
}

/**
 * Recursivly load the skeleton for this model.
 * @param node the root node, or a recursive node.
 */
void BoneModel::loadBones(aiNode * node)
{
	skeleton.importBonesFromAssimp(node);
}


void BoneModel::processNode(aiNode * node, const aiScene * scene)
{
	if (!node->mNumMeshes)
	{
		return;
	}
	if (node != nullptr)
	{
		std::cout << node->mNumMeshes << " meshes" << std::endl;
		assert(node->mNumMeshes >= 1);
		aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];
		this->mesh = this->processMesh(mesh, scene);
		std::vector<VertexBoneData> bData = processMeshBoneData(mesh);
		globalBoneData.insert(globalBoneData.end(), bData.begin(), bData.end());

		boneOffsets.resize(skeleton.getNumBones());

		for (int bone = 0; bone < mesh->mNumBones; bone++)
		{
			std::string boneName(mesh->mBones[bone]->mName.data);
			Bone * skelBone = skeleton.getBone(boneName);
			boneOffsets[skelBone->getIndex()] = glm::transpose(glm::make_mat4(&mesh->mBones[bone]->mOffsetMatrix.a1));
		}
		//Process bind matricies:

	}
}

std::vector<VertexBoneData> BoneModel::processMeshBoneData(aiMesh * mesh)
{
	std::vector<VertexBoneData> bData;
	bData.resize(mesh->mNumVertices);
	for (int bone = 0; bone < mesh->mNumBones; bone++)
	{
		std::string boneName(mesh->mBones[bone]->mName.data);
		Bone * skelBone = skeleton.getBone(boneName);
		int weight;
		for (weight = 0; weight < mesh->mBones[bone]->mNumWeights; weight++)
		{
			float boneWeight = mesh->mBones[bone]->mWeights[weight].mWeight;
			float idx = mesh->mBones[bone]->mWeights[weight].mVertexId;
			bData[idx].addBoneData(skelBone->getIndex(), boneWeight);
		}

	}
	return bData;
}
SharedPointer<ion::Scene::CSimpleMesh> BoneModel::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<f32> positions; positions.reserve(mesh->mNumVertices * 3);
	std::vector<f32> normals; normals.reserve(mesh->mNumVertices * 3);
	std::vector<f32> texCoords; texCoords.reserve(mesh->mNumVertices * 2);
	std::vector<uint> indices; indices.reserve(mesh->mNumFaces * 3);

	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		positions.push_back(mesh->mVertices[i].x);
		positions.push_back(mesh->mVertices[i].y);
		positions.push_back(mesh->mVertices[i].z);

		normals.push_back(mesh->mNormals[i].x);
		normals.push_back(mesh->mNormals[i].y);
		normals.push_back(mesh->mNormals[i].z);
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			texCoords.push_back(mesh->mTextureCoords[0][i].x);
			texCoords.push_back(mesh->mTextureCoords[0][i].y);
		}
		else
		{
			texCoords.push_back(0);
			texCoords.push_back(0);
		}
		this->minBounds.x = (float) fmin(mesh->mVertices[i].x, minBounds.x);
		this->minBounds.y = (float) fmin(mesh->mVertices[i].y, minBounds.y);
		this->minBounds.z = (float) fmin(mesh->mVertices[i].z, minBounds.z);
		this->maxBounds.x = (float) fmax(mesh->mVertices[i].x, maxBounds.x);
		this->maxBounds.y = (float) fmax(mesh->mVertices[i].y, maxBounds.y);
		this->maxBounds.z = (float) fmax(mesh->mVertices[i].z, maxBounds.z);

	}



	for (uint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return SharedPointer<ion::Scene::CSimpleMesh>(ion::Scene::CSimpleMesh::FromAttributes(indices, positions, normals, texCoords));
}

Bone * const BoneModel::getBone(std::string boneName)
{
	return skeleton.getBone(boneName);
}

const std::vector<glm::mat4> & BoneModel::getBoneOffsets()
{
	return boneOffsets;
}

void BoneModel::updateSkeleton()
{
	skeleton.finalizeAnimation();
}
