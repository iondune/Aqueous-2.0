#version 150

#define LIGHT_MAX 4


struct SPointLight
{
   vec3 Color;
   vec3 Position;
   float Radius;
};
struct SMaterial
{
   vec3 AmbientColor;
   vec3 DiffuseColor;
};

SMaterial uMaterial = SMaterial(vec3(0.05),vec3(0.3));

in vec3 fNormal;
in vec4 caustCoords;
in vec3 fWorldPosition;


uniform float elapsed;
uniform int uPointLightsCount;
uniform SPointLight uPointLights[LIGHT_MAX];
uniform sampler3D uCausticTexture;

out vec4 outColor;

float sq(float v)
{
   return v * v;
}

vec3 calcCaustColor()
{
   vec3 projCoords = caustCoords.xyz / caustCoords.w;
   projCoords = projCoords * 0.5 + 0.5;
   vec3 offset = vec3(elapsed,elapsed,0);
   projCoords.z = 0;
   projCoords += offset/ 1000.f;
   projCoords.z = elapsed;
   vec4 col = (texture(uCausticTexture,projCoords.xyz));
   
   return col.xyz;
}

void main()
{
   vec3 nNormal = normalize(fNormal);
   vec3 Diffuse = vec3(0);

   for (int i = 0; i < LIGHT_MAX && i < uPointLightsCount; ++ i)
   {
      vec3 Light = uPointLights[i].Position - fWorldPosition;
      vec3 nLight = normalize(Light);

      float Shading = clamp(dot(nNormal, nLight), 0.0, 1.0);
      float Distance = length(Light);
      float Attenuation = 1.0;// / sq(Distance / uPointLights[i].Radius + 1.0);
      Diffuse += Shading * Attenuation * uPointLights[i].Color;
   }


   outColor = vec4(uMaterial.DiffuseColor * Diffuse + uMaterial.AmbientColor + uMaterial.DiffuseColor * calcCaustColor(), 1.0);
}