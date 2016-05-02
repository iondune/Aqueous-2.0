
#version 330

in vec2 vPosition;

uniform float uScale;
uniform float uFrequency;
uniform float uHeight;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform vec3 uCameraPosition;

out vec3 vNormal;


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

	float t = 0.0;
	float g = 9.81;
	float k = uFrequency;
	float h = uHeight;
	float w = sqrt(g * k);

	vec2 gerstnerX = Gerstner(x, t, g, k, h, w);
	vec2 gerstnerY = Gerstner(y, t, g, k, h, w);

	vec4 WorldPosition = uModelMatrix * vec4(
		gerstnerX.x * uScale,
		gerstnerX.y + gerstnerY.y,
		gerstnerY.x * uScale,
		1.0);

	vNormal = vec3(0.0, 1.0, 0.0);

	gl_Position = uProjectionMatrix * uViewMatrix * WorldPosition;
}
