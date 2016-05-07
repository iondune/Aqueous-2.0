#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in ivec4 vBoneID;
layout (location = 3) in vec4  vBoneWeight;

out vec3 fNormal;
out vec3 fWorldPosition;

const int MAX_BONES = 37;

uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;

struct bindData
{
    mat4 gBone;
    mat4 gBind;
};
uniform bindData bDat[MAX_BONES];
// uniform mat4 gBones[MAX_BONES];
// uniform mat4 gBinds[MAX_BONES];


void main()
{
    mat4 BoneTransform = bDat[vBoneID[0]].gBone * bDat[vBoneID[0]].gBind * vBoneWeight[0];
    BoneTransform +=     bDat[vBoneID[1]].gBone * bDat[vBoneID[1]].gBind * vBoneWeight[1];
    BoneTransform +=     bDat[vBoneID[2]].gBone * bDat[vBoneID[2]].gBind * vBoneWeight[2];
    BoneTransform +=     bDat[vBoneID[3]].gBone * bDat[vBoneID[3]].gBind * vBoneWeight[3];

    vec4 PosL = uViewMatrix * uModelMatrix * BoneTransform * vec4(vPosition, 1.0);
    gl_Position = uProjectionMatrix * PosL;
    //fragTexCoords = TexCoord;
    vec4 NormalL = uViewMatrix * BoneTransform * vec4(vNormal, 0.0);
    fNormal = (NormalL).xyz;
    fWorldPosition = (PosL).xyz;
    
}