
#version 330

in vec2 vPosition;

uniform float uTime;
uniform float uScale;
uniform float uFrequency;
uniform float uHeight;
uniform int uSelectWave;
uniform float uSteepness;
uniform int uNumWaves;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform vec3 uCameraPosition;

out vec3 fNormal;
out vec3 fWorldPosition;
out vec2 fModelPosition;
out vec4 fScreenPosition;
out vec3 fP;


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
	fModelPosition = vPosition.xy;
	float x = vPosition.x;
	float y = vPosition.y;

	vec3 P = vec3(x, 0.0, y);

	int low = 1;
	int high = uNumWaves;
	int mid1 = low + (high - low) * 1 / 6;
	int mid2 = low + (high - low) * 2 / 6;
	int mid3 = low + (high - low) * 3 / 6;
	int mid4 = low + (high - low) * 4 / 6;
	int mid5 = low + (high - low) * 5 / 6;
	int NumWaves = high - low + 1;

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

	for (int i = low; i <= high; ++ i)
	{
		float Factor = 1.0;
		// if (i > mid1)
		// 	Factor = sqrt(0.1);
		// if (i > mid2)
		// 	Factor = sqrt(0.01);
		// if (i > mid3)
		// 	Factor = sqrt(0.001);
		// if (i > mid4)
		// 	Factor = sqrt(0.0001);
		// if (i > mid5)
		// 	Factor = sqrt(0.00001);

		float Wavelength = (rand(float(i)) * 1.5 + 0.5) * MedianWavelength * Factor * Factor;
		float Amplitude = (rand(float(i)) * 1.5 + 0.5) * MedianAmplitude * Factor;
		float Angle = (rand(float(i + 37)) * 2.0 - 1.0) * MaxDirectionVariance;

		vec2 D_i = rotate(MedianDirection, Angle);
		float A_i = Amplitude;
		float phi_i = 2.0 * pi;//0.376 + 1.329 * pow(float(i), 1.322);
		float w_i = sqrt(g * 2.0 * pi / Wavelength);
		float Q_i = uSteepness / (w_i * A_i * NumWaves);

		P.xz += Q_i * A_i * D_i * cos(w_i * dot(D_i, vec2(x, y)) + phi_i * uTime);
		P.y += A_i * sin(w_i * dot(D_i, vec2(x, y)) + phi_i * uTime);
	}

	// vec3 N = vec3(0.0, 1.0, 0.0);

	// for (int i = low; i <= high; ++ i)
	// {
	// 	float Factor = 1.0;
	// 	if (i > mid1)
	// 		Factor = sqrt(0.1);
	// 	if (i > mid2)
	// 		Factor = sqrt(0.01);
	// 	if (i > mid3)
	// 		Factor = sqrt(0.001);

	// 	float Wavelength = (rand(float(i)) * 1.5 + 0.5) * MedianWavelength * Factor * Factor;
	// 	float Amplitude = (rand(float(i)) * 1.5 + 0.5) * MedianAmplitude * Factor;
	// 	float Angle = (rand(float(i + 37)) * 2.0 - 1.0) * MaxDirectionVariance;

	// 	vec2 D_i = rotate(MedianDirection, Angle);
	// 	float A_i = Amplitude;
	// 	float phi_i = 2.0 * pi;//0.376 + 1.329 * pow(float(i), 1.322);
	// 	float w_i = sqrt(g * 2.0 * pi / Wavelength);
	// 	float Q_i = uSteepness / (w_i * A_i * NumWaves);

	// 	float WA = w_i * A_i;
	// 	float S = sin(w_i * dot(D_i, P.xz) + phi_i * uTime);
	// 	float C = cos(w_i * dot(D_i, P.xz) + phi_i * uTime);

	// 	N.xz += D_i * WA * C;
	// 	N.y -= Q_i * WA * S;
	// }

	vec4 WorldPosition = uModelMatrix * vec4(
		P * vec3(uScale, 1.0, uScale),
		1.0);
	fP = P;

	// fNormal = N;
	fWorldPosition = WorldPosition.xyz;

	gl_Position = uProjectionMatrix * uViewMatrix * WorldPosition;
	fScreenPosition = gl_Position;
}
