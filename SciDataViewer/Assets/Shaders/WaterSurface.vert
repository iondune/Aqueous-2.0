
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


vec2 Gerstner(float x, float t, float g, float k, float h, float w)
{
	return vec2(
		x + h * sin(w * t - k * x),
		h * cos(w * t - k * x)
		);
}

vec3 GerstnerXY(float x, float y)
{
	float t = uTime;
	float g = 9.81;
	float k = uFrequency;
	float h = uHeight;
	float w = sqrt(g * k);

	vec2 gerstnerX = Gerstner(x, t, g, k, h, w);
	vec2 gerstnerY = Gerstner(y, t, g, k, h, w);

	return vec3(
		gerstnerX.x * uScale,
		gerstnerX.y + gerstnerY.y,
		gerstnerY.x * uScale
		);
}

vec3 GerstnerNormal(float x, float y)
{
	const float Epsilon = 0.1;

	vec3 s01 = GerstnerXY(x - Epsilon, y);
	vec3 s21 = GerstnerXY(x + Epsilon, y);
	vec3 s10 = GerstnerXY(x, y - Epsilon);
	vec3 s12 = GerstnerXY(x, y + Epsilon);

	vec3 va = normalize(vec3(s21.x - s01.x, s21.y - s01.y, 0.0));
	vec3 vb = normalize(vec3(0.0, s12.y - s10.y, s12.z - s10.z));
	return normalize(cross(vb, va));
}


void main()
{
	float x = vPosition.x;
	float y = vPosition.y;

	vec3 P = vec3(x, 0.0, y);

	int low = 1;
	int high = 15;
	int NumWaves = high - low + 1;

	if (uSelectWave > 0)
	{
		low = high = uSelectWave;
		NumWaves = 1;
	}

	for (int i = low; i <= high; ++ i)
	{
		vec2 D_i = normalize(vec2(1.0, 0.8 * float(i) / float(high)));
		if (i % 2 == 0)
			D_i = D_i.yx;
		float A_i = 3.0 * uHeight / pow(float(i), 2.3564);
		float phi_i = 0.376 + 1.329 * pow(float(i), 1.322);
		float w_i = uFrequency / 20.0 * float(i);
		float Q_i = uSteepness / (w_i * A_i * NumWaves);

		P.xz += Q_i * A_i * D_i * cos(w_i * dot(D_i, vec2(x, y)) + phi_i * uTime);
		P.y += A_i * sin(w_i * dot(D_i, vec2(x, y)) + phi_i * uTime);
	}

	vec3 N = vec3(0.0, 1.0, 0.0);

	for (int i = low; i <= high; ++ i)
	{
		vec2 D_i = normalize(vec2(1.0, 0.8 * float(i) / float(high)));
		if (i % 2 == 0)
			D_i = D_i.yx;
		float A_i = 3.0 * uHeight / float(i);
		float phi_i = 0.376 + 1.329 * pow(float(i), 1.322);
		float w_i = uFrequency / 20.0 * float(i);
		float Q_i = uSteepness / (w_i * A_i * NumWaves);

		float WA = w_i * A_i;
		float S = sin(w_i * dot(D_i, P.xz) + phi_i * uTime);
		float C = cos(w_i * dot(D_i, P.xz) + phi_i * uTime);

		N.xz += D_i * WA * C;
		N.y -= Q_i * WA * S;
	}

	vec4 WorldPosition = uModelMatrix * vec4(
		// GerstnerXY(x, y),
		P * vec3(uScale, 1.0, uScale),
		1.0);

	vNormal = N;
	// vNormal = GerstnerNormal(x, y);
	vWorldPosition = WorldPosition.xyz;

	gl_Position = uProjectionMatrix * uViewMatrix * WorldPosition;
}
