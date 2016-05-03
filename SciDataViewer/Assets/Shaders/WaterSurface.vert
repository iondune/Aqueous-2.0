
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

const vec2 D[3] = vec2[] (normalize(vec2(1.0, 0.0)), normalize(vec2(0.0, 1.0)), normalize(vec2(1.0, 1.0)));
float Amplitude[3] = float[] (3.0 * uHeight, 3.0 * uHeight, 1.0 * uHeight);
const float phi[3] = float[] (1.0, 1.0, 1.0);
float w[3] = float[] (uFrequency / 20.0, uFrequency / 20.0, uFrequency / 40.0);

vec3 GerstnerNew(float x, float y)
{
	vec3 P = vec3(x, 0.0, y);

	int low = 0;
	int high = 1;

	if (uSelectWave != -1)
	{
		low = high = uSelectWave;
	}

	for (int i = low; i <= high; ++ i)
	{
		float Q = uSteepness / (w[i] * Amplitude[i]);
		P.xz += Q * Amplitude[i] * D[i] * cos(w[i] * dot(D[i], vec2(x, y)) + phi[i] * uTime);
		P.y += Amplitude[i] * sin(w[i] * dot(D[i], vec2(x, y)) + phi[i] * uTime);
	}
	return P;
}

vec2 Gerstner(float x, float t, float g, float k, float h, float w)
{
	return vec2(
		x + h * sin(w * t - k * x),
		h * cos(w * t - k * x)
		);
}

void main()
{

	float x = vPosition.x;
	float y = vPosition.y;

	float t = uTime;
	float g = 9.81;
	float k = uFrequency;
	float h = uHeight;
	float w = sqrt(g * k);

	vec2 gerstnerX = Gerstner(x, t, g, k, h, w);
	vec2 gerstnerY = Gerstner(y, t, g, k, h, w);

	vec4 WorldPosition = uModelMatrix * vec4(
		// gerstnerX.x * uScale,
		// gerstnerX.y + gerstnerY.y,
		// gerstnerY.x * uScale,
		GerstnerNew(x, y) * vec3(uScale, 1.0, uScale),
		1.0);

	vNormal = vec3(0.0, 1.0, 0.0);

	gl_Position = uProjectionMatrix * uViewMatrix * WorldPosition;
}
