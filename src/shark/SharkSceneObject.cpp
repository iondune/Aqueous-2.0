#include "shark/SharkSceneObject.h"
#include <ionGraphics.h>
#include <imgui/imgui.h>
#include "shark/SharkSpineOscilator.h"
#define NUM_OSC_BONES 9

float getMaxSpeed()
{
   return 2;
}
float getMinSpeed()
{
   return 0.2;
}


float getFrequency(float length, float speed)
{
   float minHz;
   float maxHz;
   if(length < 60)
   {
      minHz = 1.25;
      maxHz = 2.25;
   }
   else if(length < 90)
   {
      minHz = 1.0;
      maxHz = 1.75;
   }
   else
   {
      minHz = 0.75;
      maxHz = 1.20;
   }
   return minHz + (maxHz - minHz) * (speed - getMinSpeed()) / (getMaxSpeed() - getMinSpeed());

}
float getAmplitude(float length, float speed)
{
   float minHz;
   float maxHz;
   if(length < 60)
   {
      minHz = 1.25;
      maxHz = 2.25;
   }
   else if(length < 90)
   {
      minHz = 1.0;
      maxHz = 1.75;
   }
   else
   {
      minHz = 0.75;
      maxHz = 1.20;
   }
   //Aimplitude of oscilation is based off of full spine, not just one section
   float maxA = length/(5*2);
   return maxA * (getFrequency(length, speed) - minHz)/(maxHz - minHz);

}


SharkSceneObject::SharkSceneObject(std::string ModelName):
Model(ModelName),
InternalTime(0),
parentTransforms(true)
{
   //Load mesh
   this->SetMesh(&(*Model.mesh));
   for(int i = 1; i <= NUM_OSC_BONES; i++)
   {
      SharkSpineOscilator osc("Spine" + std::to_string(i),i);
      osc.setPhase(-M_PI/NUM_OSC_BONES*i);
      osc.setAlpha(0.15 * (i < 3 ? 1.5 : i)); //Model Subcarangiform motion by osiclating the last 2/3 more than the first

      oscilators.push_back(osc);

   }

}
SharkSceneObject::~SharkSceneObject()
{

}
void SharkSceneObject::Load(ion::Scene::CRenderPass * RenderPass)
{
   
   SharedPointer<ion::Graphics::IPipelineState> PipelineState;
   if (! TryMapAccess(PipelineStates, RenderPass, PipelineState))
   {
      PipelineStates[RenderPass] = PipelineState = RenderPass->GetGraphicsContext()->CreatePipelineState();
   }
   PipelineState->SetVertexBuffer(1, Model.boneData);
   //Offer pointers to the bones and binds.
   std::vector<glm::mat4> offsets = Model.getBoneOffsets();
   std::vector<Bone> & bones = Model.getAllBones();
   assert(offsets.size() == bones.size());
   for(int i = 0; i < bones.size(); i++)
   {

      std::shared_ptr<ion::Graphics::IUniform> bindUniform =
      std::make_shared<ion::Graphics::CUniformValue<glm::mat4>>(offsets[i]);
      auto boneUniform =
      std::make_shared<ion::Graphics::CUniformValue<glm::mat4>>(bones[i].getAnimMatrix());

      boneUniforms.push_back(boneUniform);
      SetUniform("bDat[" + std::to_string(i) + "].gBind", bindUniform);
      SetUniform("bDat[" + std::to_string(i) + "].gBone", boneUniform);
   }
   CSimpleMeshSceneObject::Load(RenderPass);
   RenderPass->PreparePipelineStateForRendering(PipelineState, this);
}

void SharkSceneObject::setTime(f64 newTime)
{
   this->InternalTime = newTime;
}


void SharkSceneObject::update(KeySpline & spline, f64 dt){
   static float targSpeed = 1.0;
   static float speed = 1.0;
   
   InternalTime += speed * dt;
   static float length = 100;
   float s = InternalTime;
   bool smoothTurn = true;
   //Get spline Transform
   if(smoothTurn)
   {
      SetTransformation(glm::mat4(1.0));
      for(int i = 0; i < NUM_OSC_BONES; i++)
      {
               //Choose U offsets

         float stepU = s-(float)i*(10*(length/100)/Model.getWidth())/NUM_OSC_BONES;
         stepU = spline.sToU(stepU);
         Transform tns = spline.transformAt(stepU);
               //Model.transform.setPosition(tns.getPosition());
               //Model.transform.setRotation(tns.getRotation());
         Transform boneTransform;
         //float sclRecip = 1/(length/100.f/Model.getWidth());
            
               //Swizzel the positions
         boneTransform.setPosition(glm::vec3(tns.getPosition().z,tns.getPosition().y,tns.getPosition().x));
         boneTransform.lookAlong(glm::vec3(-tns.forward().x,tns.forward().y,tns.forward().z));

            
         //boneTransform.rotate(M_PI/2,tns.forward());
         boneTransform.rotate(M_PI, glm::vec3(0,1,0));
         
         //boneTransform.setRotation()
         //boneTransform.setPosition(glm::vec3(i,0,0));
         //tns.lookAlong(glm::vec3(tns.forward().z,tns.forward().y,tns.forward().x));
               //boneTransform.setScale(glm::vec3(5));
         oscilators[i].setOffset(boneTransform.getMatrix());
         //Model.getBone("Spine" + std::to_string(i))->transform.setTransformation(boneTransform.getMatrix());
         //oscilators[i].getBone(Model)->setApplyParentTransform(true);
      }
   }

   else
   {
      float stepU = spline.sToU(s);
      Transform tns = spline.transformAt(stepU);
      SetTransformation(tns.getMatrix());
   }



   ImGui::Begin("Shark");
   ImGui::SliderFloat("length (cm)", &length,  30,  121, "%.3f",  1.0f);
   //ApplyScaleFactor(glm::vec3(length/100.f/Model.getWidth()));
   ImGui::End();

   ImGui::Begin("Movement");
   {
      ImGui::InputFloat("Speed", &targSpeed,0.01f,1.0f);
      float maxSpeed = getMaxSpeed();
         //Clamp the target speed value
      targSpeed = std::min(getMaxSpeed(),targSpeed);
      targSpeed = std::max(getMinSpeed(),targSpeed);

      float accel = (targSpeed - speed) * dt;

      speed+= accel;
         //Set Frequency and Amplitude from speed
         //Calculate velocity Shark is appx 120 cm
      float freq = getFrequency(length, speed);
      float ampl = getAmplitude(length, speed)/70.f;

      for (std::vector<SharkSpineOscilator>::iterator osc = oscilators.begin(); osc != oscilators.end(); ++osc)
      {
            osc->setHz(freq);//Oscilating frequency function of Shark len and speed
            osc->setAlpha(ampl * (osc->getID() < 4 ? 0.9f :  1.1)); //Model Subcarangiform motion by osiclating the last 2/3 more than the first
      }

      //u += dt * speed;

	  float t = InternalTime;
	  if (ImGui::SliderFloat("Time", &t, 0.0f, 100.0f))
	  {
		  InternalTime = t;
	  }
   }
   ImGui::End();

   for (std::vector<SharkSpineOscilator>::iterator osc = oscilators.begin(); osc != oscilators.end(); ++osc)
   {
         //osc->handleGUI();
      osc->update(dt);
      osc->apply(Model);
   }
   Model.updateSkeleton();
   if(boneUniforms.size() == 0)
   {
      TriggerReload();
   }
   else
   {
      std::vector<Bone> & bones = Model.getAllBones();
      for(int i = 0; i < bones.size(); i++)
      {
         *boneUniforms[i] = bones[i].getAnimMatrix();

      }      
   }

}