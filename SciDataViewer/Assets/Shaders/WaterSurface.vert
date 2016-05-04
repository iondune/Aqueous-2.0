
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

out vec3 vNormal;
out vec3 vWorldPosition;


float rand(vec2 co)
{
	return fract(sin(dot(co.xy ,vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	float x = vPosition.x;
	float y = vPosition.y;

	vec3 P = vec3(x, 0.0, y);

	int low = 1;
	int high = uNumWaves;
	int NumWaves = high - low + 1;

	if (uSelectWave > 0)
	{
		low = high = uSelectWave;
		NumWaves = 1;
	}

	NumWaves += 1;

	for (int i = low; i <= high; ++ i)
	{
		vec2 D_i = normalize(vec2(1.0, rand(vec2(float(i), 2.0 * float(i)))));
		float A_i = 3.0 * uHeight / pow(float(i), 2.3564);
		float phi_i = 0.376 + 1.329 * pow(float(i), 1.322);
		float w_i = uFrequency / 20.0 * float(i);
		float Q_i = uSteepness / (w_i * A_i * NumWaves);

		P.xz += Q_i * A_i * D_i * cos(w_i * dot(D_i, vec2(x, y)) + phi_i * uTime);
		P.y += A_i * sin(w_i * dot(D_i, vec2(x, y)) + phi_i * uTime);

		if (i == low)
		{
			D_i = D_i.yx * vec2(-1.0, 1.0);

			P.xz += Q_i * A_i * D_i * cos(w_i * dot(D_i, vec2(x, y)) + phi_i * uTime);
			P.y += A_i * sin(w_i * dot(D_i, vec2(x, y)) + phi_i * uTime);
		}
	}

	vec3 N = vec3(0.0, 1.0, 0.0);

	for (int i = low; i <= high; ++ i)
	{
		vec2 D_i = normalize(vec2(1.0, rand(vec2(float(i), 2.0 * float(i)))));
		float A_i = 3.0 * uHeight / float(i);
		float phi_i = 0.376 + 1.329 * pow(float(i), 1.322);
		float w_i = uFrequency / 20.0 * float(i);
		float Q_i = uSteepness / (w_i * A_i * NumWaves);

		float WA = w_i * A_i;
		float S = sin(w_i * dot(D_i, P.xz) + phi_i * uTime);
		float C = cos(w_i * dot(D_i, P.xz) + phi_i * uTime);

		N.xz += D_i * WA * C;
		N.y -= Q_i * WA * S;

		if (i == low)
		{
			D_i = D_i.yx * vec2(-1.0, 1.0);
			S = sin(w_i * dot(D_i, P.xz) + phi_i * uTime);
			C = cos(w_i * dot(D_i, P.xz) + phi_i * uTime);

			N.xz += D_i * WA * C;
			N.y -= Q_i * WA * S;
		}
	}

	vec4 WorldPosition = uModelMatrix * vec4(
		P * vec3(uScale, 1.0, uScale),
		1.0);

	vNormal = N;
	vWorldPosition = WorldPosition.xyz;

	gl_Position = uProjectionMatrix * uViewMatrix * WorldPosition;
}
