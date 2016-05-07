#include "SharkSpineOscilator.h"
#include <imgui/imgui.h>
#include "shark/Transform.h"
#include <glm/gtc/matrix_access.hpp>
SharkSpineOscilator::SharkSpineOscilator(std::string boneName, int id):
boneName(boneName),
boneId(id),
offset(1.0)
{
   dTau = targDTau = 1.0;
   alpha = targAlpha = 0.0;
   phase = targPhase = 0.0;
   tau = 0.0;
}

void SharkSpineOscilator::SharkSpineOscilator::apply(BoneModel & model)
{
   Bone * const bonePtr = model.getBone(boneName);
   if(bonePtr != nullptr)
   {
      Transform t;
      float finalPos = 0;
      float finalAngl = 0;
      for(int harm = 1; harm <= harmonic; harm++)
      {
         finalPos +=   sin(tau*harm + phase) * alpha/harm;
         finalAngl += -harm*cos(tau*harm + phase) * alpha/harm;
      }
      t.setRotation(finalAngl, glm::vec3(1,0,0));
      t.setPosition(glm::vec3(0,0,finalPos));

      //axial angle has a period of 2x the original angle, and a frequency of "A fraction" of the original.
      //t.rotate(sin(2*tau+phase) * alpha * 0.1f,glm::vec3(0,0,1));
      glm::mat4 offsetMtx = glm::column(bonePtr->getOffsetMatrix(),3,glm::vec4(0,0,0,1));
      bonePtr->setAnimatedTransform( offset * offsetMtx *  t.getMatrix());
   }
   else
   {
      std::cout << "Could not find bone" << std::endl;
   }
}
void SharkSpineOscilator::SharkSpineOscilator::update(float dt)
{
    
   dTau  +=  (targDTau - dTau) * 0.01; //Exponential interpolation
   alpha += (targAlpha - alpha) * 0.01;
   phase += (targPhase - phase) * 0.01;
   tau += (Constants32::Pi*2 * dTau) * dt;
}

void SharkSpineOscilator::handleGUI()
{
   ImGui::Text(boneName.c_str());
   ImGui::Value("alpha", targAlpha);
   ImGui::Value("dTau", targDTau);
   ImGui::Value("phase", targPhase);

   
}
void SharkSpineOscilator::setAlpha(float newAlpha)
{
   targAlpha = newAlpha;
}
void SharkSpineOscilator::setHz(float dTau)
{
   targDTau = dTau;
}
void SharkSpineOscilator::setHarmonic(int harmonic)
{
   this->harmonic = harmonic;
}
void SharkSpineOscilator::setPhase(float newPhase)
{
   targPhase = newPhase;
}
void SharkSpineOscilator::setTau(float tau)
{
   this->tau = tau;
}

void SharkSpineOscilator::setOffset(glm::mat4 offset)
{
   this->offset = offset;
}

int SharkSpineOscilator::getID()
{
   return boneId;
}

Bone * SharkSpineOscilator::getBone(BoneModel & model)
{
   return model.getBone(boneName);
}