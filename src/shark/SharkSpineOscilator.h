#ifndef __SHARK_SPINE_OSCILATOR_H__
#define __SHARK_SPINE_OSCILATOR_H__
#include "Model.h"
/**
 * Model 
 */
class SharkSpineOscilator
{

public:
   SharkSpineOscilator(std::string spineName, int id);
   void apply(BoneModel & model);
   void update(float dt);
   void setAlpha(float newAlpha);
   void setHz(float newTau);
   void setTau(float tau);
   void setPhase(float phase);
   void setHarmonic(int harmonic);
   void handleGUI();
   void setOffset(glm::mat4 offset);
   Bone * getBone(BoneModel & model);
   int getID();
private:
   glm::mat4 offset;
   std::string boneName;
   int boneId;
   float targDTau; //Ratio of dT to dTau
   float targAlpha;
   float dTau;
   float alpha;
   float tau;
   float phase, targPhase;
   int harmonic = 1;

};
#endif