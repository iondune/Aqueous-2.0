
#pragma once

#include <ionCore.h>
#include <ionScene.h>

#include "shark/Model.h"
#include "SharkSpineOscilator.h"
#include "spline/CKeySpline.h"


class SharkSceneObject : public ion::Scene::CSimpleMeshSceneObject
{

public:

	SharkSceneObject(std::string modelName);
	~SharkSceneObject();
	void update(CKeySpline & spline, f64 const deltaTime);
	virtual void Load(ion::Scene::CRenderPass * renderpass);
	void setTime(f64 newTime);
	void applyParentTransforms(bool toggle) { parentTransforms = toggle; }
	//void setOffset(glm::mat4 offset);

private:

	std::vector<glm::mat4> offsetRotations;
	BoneModel Model;
	std::vector<SharkSpineOscilator> oscilators;
	std::vector<std::shared_ptr<ion::Graphics::CUniformValue<glm::mat4>>> boneUniforms;
	SharedPointer<ion::Graphics::CUniformValue<float>> elapsedTime;
	f64 InternalTime;

	glm::mat4 offset;
	bool parentTransforms;
};
