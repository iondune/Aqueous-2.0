
#version 330

#define LIGHT_MAX 4

struct SPointLight
{
	vec3 Color;
	float Radius;
};

in vec3 vNormal;
in vec3 vWorldPosition;

uniform int uPointLightCount;
uniform SPointLight uPointLights[LIGHT_MAX];
uniform vec3 uCameraPosition;
uniform samplerCube uSkyBox;

out vec4 outColor;


float sq(float v)
{
	return v * v;
}

void main()
{
	const vec3 AmbientColor = 0.4 * vec3(0.0, 1.0, 1.0);
	const vec3 DiffuseColor = 0.6 * vec3(0.0, 0.5, 1.0);
	const vec3 SpecularColor = 0.5 * vec3(1.0, 1.0, 1.0);
	const float Shininess = 100.0;
	const float Reflection = 0.5;

	vec3 nNormal = normalize(vNormal);
	vec3 nView = normalize(uCameraPosition - vWorldPosition);
	vec3 nReflect = normalize(reflect(-nView, nNormal));

	vec3 Diffuse = vec3(0);
	vec3 Specular = vec3(0);

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

	//for (int i = 0; i < LIGHT_MAX && i < uLightCount; ++ i)
	{
		vec3 nLight = -normalize(vec3(-1.0, -6.0, 0.0));
		vec3 nHalf = normalize(nLight + nView);
		float Lambertian = clamp(dot(nNormal, nLight), 0.0, 1.0);
		float BlinnPhong = pow(clamp(dot(nNormal, nHalf), 0.0, 1.0), Shininess);
		Diffuse += Lambertian;
		Specular += BlinnPhong;
	}


	outColor = vec4(Specular * SpecularColor + Diffuse * DiffuseColor + AmbientColor, 1.0);
	outColor.rgb *= (1.0 - Reflection);
	outColor.rgb += Reflection * texture(uSkyBox, nReflect).rgb;
	// outColor = vec4(Diffuse, 1.0);
	// outColor = vec4(vNormal * 0.5 + vec3(0.5), 1);
}
