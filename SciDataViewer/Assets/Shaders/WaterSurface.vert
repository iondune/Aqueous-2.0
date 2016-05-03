
#version 330

in vec2 vPosition;

uniform float uTime;
uniform float uScale;
uniform float uFrequency;
uniform float uHeight;
uniform int uSelectWave;
uniform float uSteepness;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform vec3 uCameraPosition;

out vec3 vNormal;
out vec3 vWorldPosition;


void main()
{
	const vec2 D[3] = vec2[] (normalize(vec2(1.0, 0.5)), normalize(vec2(0.5, 1.0)), normalize(vec2(1.0, 1.0)));
	float Amplitude[3] = float[] (3.0 * uHeight, 3.0 * uHeight, 0.4 * uHeight);
	const float phi[3] = float[] (1.0, 1.0, 5.0);
	float w[3] = float[] (uFrequency / 20.0, uFrequency / 20.0, uFrequency / 4.0);

	float x = vPosition.x;
	float y = vPosition.y;

	vec3 P = vec3(x, 0.0, y);

	int low = 0;
	int high = 2;
	int NumWaves = high - low + 1;

	if (uSelectWave != -1)
	{
		low = high = uSelectWave;
		NumWaves = 1;
	}

	for (int i = low; i <= high; ++ i)
	{
		float Q = uSteepness / (w[i] * Amplitude[i] * NumWaves);

		P.xz += Q * Amplitude[i] * D[i] * cos(w[i] * dot(D[i], vec2(x, y)) + phi[i] * uTime);
		P.y += Amplitude[i] * sin(w[i] * dot(D[i], vec2(x, y)) + phi[i] * uTime);
	}

	vec3 N = vec3(0.0, 1.0, 0.0);

	for (int i = low; i <= high; ++ i)
	{
		float Q = uSteepness / (w[i] * Amplitude[i] * NumWaves);
		float WA = w[i] * Amplitude[i];
		float S = sin(w[i] * dot(D[i], P.xz) + phi[i] * uTime);
		float C = cos(w[i] * dot(D[i], P.xz) + phi[i] * uTime);

		N.xz += D[i] * WA * C;
		N.y -= Q * WA * S;
	}

	vec4 WorldPosition = uModelMatrix * vec4(
		P * vec3(uScale, 1.0, uScale),
		1.0);

	vNormal = N;
	vWorldPosition = WorldPosition.xyz;

	gl_Position = uProjectionMatrix * uViewMatrix * WorldPosition;
}
