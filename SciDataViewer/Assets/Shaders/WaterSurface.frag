
#version 330

#define LIGHT_MAX 4

struct SPointLight
{
	vec3 Color;
	float Radius;
};

in vec3 fNormal;
in vec3 fWorldPosition;
in vec2 fModelPosition;
in vec3 fP;

uniform float uTime;
uniform float uScale;
uniform float uFrequency;
uniform float uHeight;
uniform int uSelectWave;
uniform float uSteepness;
uniform int uNumWaves;

uniform int uPointLightCount;
uniform SPointLight uPointLights[LIGHT_MAX];
uniform vec3 uCameraPosition;
uniform samplerCube uSkyBox;

out vec4 outColor;


float sq(float v)
{
	return v * v;
}

float rand(vec2 co)
{
	return fract(sin(dot(co.xy ,vec2(12.9898, 78.233))) * 43758.5453);
}

float rand(float c)
{
	return fract(sin(dot(vec2(c - 39.01298, c + 809.203), vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 rotate(vec2 v, float a)
{
	float cs = cos(a);
	float sn = sin(a);
	return vec2(
		v.x * cs - v.y * sn,
		v.x * sn + v.y * cs
		);
}


void main()
{
	float x = fModelPosition.x;
	float y = fModelPosition.y;

	vec3 P = vec3(x, 0.0, y);

	int low = 1;
	int high = uNumWaves;
	int mid1 = low + (high - low) * 1 / 6;
	int mid2 = low + (high - low) * 2 / 6;
	int mid3 = low + (high - low) * 3 / 6;
	int mid4 = low + (high - low) * 4 / 6;
	int mid5 = low + (high - low) * 5 / 6;
	int NumWaves = high - low + 1;

	vec3 CameraVector = uCameraPosition - fWorldPosition;
	float CameraDistance = length(CameraVector);
	int ArtificialCutoff = clamp(int((1.0 - CameraDistance / 1500.0) * float(high)), mid3, high);

	if (uSelectWave > 0)
	{
		low = high = uSelectWave;
		NumWaves = 1;
	}

	const float pi = 3.14159;
	const float g = 9.8;
	const float MedianWavelength = 900.0;
	float MedianAmplitude = 1.25 * uHeight;
	const vec2 MedianDirection = normalize(vec2(1.0, 1.0));
	const float MaxDirectionVariance = pi / 2.0;

	vec3 N = vec3(0.0, 1.0, 0.0);

	for (int i = low; i <= high && i <= ArtificialCutoff; ++ i)
	{
		float Factor = 1.0;
		if (i > mid1)
			Factor = sqrt(0.1);
		if (i > mid2)
			Factor = sqrt(0.01);
		if (i > mid3)
			Factor = sqrt(0.001);
		if (i > mid4)
			Factor = sqrt(0.0001);
		if (i > mid5)
			Factor = sqrt(0.00001);

		float Wavelength = (rand(float(i)) * 1.5 + 0.5) * MedianWavelength * Factor * Factor;
		float Amplitude = (rand(float(i)) * 1.5 + 0.5) * MedianAmplitude * Factor;
		float Angle = (rand(float(i + 37)) * 2.0 - 1.0) * MaxDirectionVariance;

		vec2 D_i = rotate(MedianDirection, Angle);
		float A_i = Amplitude;
		float phi_i = 2.0 * pi;//0.376 + 1.329 * pow(float(i), 1.322);
		float w_i = sqrt(g * 2.0 * pi / Wavelength);
		float Q_i = uSteepness / (w_i * A_i * NumWaves);

		float WA = w_i * A_i;
		float S = sin(w_i * dot(D_i, P.xz) + phi_i * uTime);
		float C = cos(w_i * dot(D_i, P.xz) + phi_i * uTime);

		N.xz += D_i * WA * C;
		N.y -= Q_i * WA * S;
	}

	const vec3 AmbientColor = vec3(20.0 / 255.0, 45.0 / 255.0, 60.0 / 255.0);
	const vec3 DiffuseColor = vec3(116.0 / 255.0, 155.0 / 255.0, 175.0 / 255.0) - AmbientColor;
	const vec3 SpecularColor = 0.5 * vec3(1.0, 1.0, 1.0);
	const float Shininess = 10.0;
	const float Reflection = 0.1;
	const float Filter = 0.5;
	const float IoR = 1.00 / 1.2;

	vec3 nNormal = normalize(N);
	vec3 nView = normalize(uCameraPosition - fWorldPosition);
	vec3 nReflect = normalize(reflect(-nView, nNormal));
	vec3 nRefract = normalize(refract(-nView, nNormal, IoR));

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
		vec3 nLight = normalize(100.0 * vec3(1.0, 6.0, 0.0) * fWorldPosition);
		vec3 nHalf = normalize(nLight + nView);
		float Lambertian = clamp(dot(nNormal, nLight), 0.0, 1.0);
		float BlinnPhong = pow(clamp(dot(nNormal, nHalf), 0.0, 1.0), Shininess);
		Diffuse += Lambertian;
		Specular += BlinnPhong;
	}


	outColor = vec4(Specular * SpecularColor + Diffuse * DiffuseColor + AmbientColor, 1.0);
	outColor.rgb *= (1.0 - Reflection);
	outColor.rgb += Reflection * texture(uSkyBox, nReflect).rgb;
	// outColor.rgb *= (1.0 - Filter);
	// outColor.rgb += Filter * texture(uSkyBox, nRefract).rgb;
	// outColor = vec4(Diffuse, 1.0);
	// outColor = vec4(vNormal * 0.5 + vec3(0.5), 1);
	// outColor = vec4(vec3(0.0), 1.0);
}
