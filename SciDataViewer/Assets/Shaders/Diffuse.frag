#version 150

#define LIGHT_MAX 4

struct SMaterial
{
	vec3 AmbientColor;
	vec3 DiffuseColor;
};

struct SPointLight
{
	vec3 Color;
	vec3 Position;
	float Radius;
};

in vec3 fNormal;
in vec3 fWorldPosition;

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
	vec3 nNormal = normalize(fNormal);
	vec3 Diffuse = vec3(0);

	for (int i = 0; i < LIGHT_MAX && i < uPointLightCount; ++ i)
	{
		vec3 Light = uPointLights[i].Position - fWorldPosition;
		vec3 nLight = normalize(Light);

		float Shading = clamp(dot(nNormal, nLight), 0.0, 1.0);
		float Distance = length(Light);
		float Attenuation = 1.0 / sq(Distance / uPointLights[i].Radius + 1.0);
		Diffuse += Shading * Attenuation * uPointLights[i].Color;
	}

	outColor = vec4(uMaterial.DiffuseColor * Diffuse + uMaterial.AmbientColor, 1.0);
}
