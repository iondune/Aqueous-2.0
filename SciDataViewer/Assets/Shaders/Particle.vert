#version 150

in vec2 vPosition;
in vec3 vInstancePosition;
in vec3 vInstanceColor;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform float uParticleSize;

out vec3 fColor;


void main()
{
	vec3 CameraRight = vec3(uViewMatrix[0][0], uViewMatrix[1][0], uViewMatrix[2][0]);
	vec3 CameraUp = vec3(uViewMatrix[0][1], uViewMatrix[1][1], uViewMatrix[2][1]);

	vec3 Vertex =
		CameraRight * vPosition.x * uParticleSize +
		CameraUp * vPosition.y * uParticleSize;

	Vertex += vInstanceLocation;

	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(Vertex, 1.0);
	fColor = vInstanceColor;
}
