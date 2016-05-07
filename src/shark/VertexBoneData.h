#ifndef __VERTEX_BONE_DATA_H__
#define __VERTEX_BONE_DATA_H__
//Global number of bones per vertex
#define NUM_BONES_PER_VERTEX 4
#include <cassert>
#include <iostream>
#include <ionCore.h>
#include <ionGraphics.h>

struct VertexBoneData
{

   VertexBoneData(){
      for(int i = 0; i < NUM_BONES_PER_VERTEX; i++)
      {
         boneIds[i] = 0;
         boneWeights[i] = 0.0;
      }
   }

   uint boneIds[NUM_BONES_PER_VERTEX];
   f32  boneWeights[NUM_BONES_PER_VERTEX];

   void addBoneData(int Bone, float weight)
   {
      for(uint i = 0; i < NUM_BONES_PER_VERTEX; i++)
      {
         if(boneWeights[i] == 0.0){
            boneIds[i] = Bone;
            boneWeights[i] = weight;

            return;
         }
      }
      std::cerr << "Too many bones for a vertex!" << std::endl;
      assert(false); //Too many bones!
   }

   bool isEmpty()
   {
      return boneWeights[0] == 0.0;
   }
   /**
    * Bundle up the data in a buffer
    */
   static SharedPointer<ion::Graphics::IVertexBuffer> CreateVertexBuffer(const std::vector<VertexBoneData> data)
   {
      static SingletonPointer<ion::CGraphicsAPI> GraphicsAPI;

      SharedPointer<ion::Graphics::IVertexBuffer> VertexBuffer = GraphicsAPI->CreateVertexBuffer();
      VertexBuffer->UploadData(data);
      ion::Graphics::SInputLayoutElement InputLayout[] =
      {
         { "vBoneID",        4, ion::Graphics::EAttributeType::Int },
         { "vBoneWeight",    4, ion::Graphics::EAttributeType::Float },
      };
      VertexBuffer->SetInputLayout(InputLayout, ION_ARRAYSIZE(InputLayout));
      return VertexBuffer;
   }


};
#endif