
#version 330

#define LIGHT_MAX 4

struct SMaterial
{
	vec3 AmbientColor;
	vec3 DiffuseColor;
};

struct SPointLight
{
	vec3 Color;
	float Radius;
};

in vec3 vNormal;

uniform int uPointLightCount;
uniform SPointLight uPointLights[LIGHT_MAX];
uniform SMaterial uMaterial;

out vec4 outColor;


float sq(float v)
{
	return v * v;
}

void main()
{
	vec3 Diffuse = vec3(0);

	// for (int i = 0; i < LIGHT_MAX && i < uLightCount; ++ i)
	// {
	// 	vec3 nLight = normalize(vLight[i]);
	// 	vec3 nNormal = normalize(vNormal);
	// 	vec3 Reflection = reflect(-nLight, nNormal);

	// 	float Shading = clamp(dot(nNormal, nLight), 0.0, 1.0);
	// 	float Distance = length(vLight[i]);
	// 	float Attenuation = 1.0 / sq(Distance / uLights[i].Radius + 1);
	// 	Diffuse += Shading * Attenuation * uLights[i].Color;
	// }

	outColor = vec4(Diffuse * uMaterial.DiffuseColor + uMaterial.AmbientColor, 1);
}
