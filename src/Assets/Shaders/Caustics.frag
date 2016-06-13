#version 330 core

uniform float elapsed;
uniform sampler3D causticTexture;


in vec4 fragPos;
in vec4 caustCoords;
out vec4 outColor;

float maxDepth = 15;

vec3 calcCaustColor()
{
   vec3 projCoords = caustCoords.xyz / caustCoords.w;
   projCoords = projCoords * 0.5 + 0.5;
   vec3 offset = vec3(elapsed,elapsed,0);
   
   projCoords += offset/ 1000.f;
   projCoords.z = elapsed;
   vec4 col = (texture(caustA,projCoords.xyz));
   
   return col.xyz;
}

void main()
{
   vec3 diffColor = vec3(0.92,0.78,0.68)/4;
   vec3 caustColor = calcCaustColor();
   vec3 fogColor = vec3(0.3,0.3,0.8);
	outColor = vec4(mix(diffColor + caustColor,fogColor,clamp(0,1,-fragPos.z/maxDepth)), 1.0);
}
