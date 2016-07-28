#version 150

in vec3 vPosition;
in vec3 vNormal;

uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;
uniform mat4 uViewMatrix;

uniform mat4 uProjectionMatrix;

out vec3 fNormal;
out vec3 fWorldPosition;


void main()
{
	vec4 WorldPosition = uModelMatrix * vec4(vPosition, 1.0);

	fWorldPosition = WorldPosition.xyz;
	fNormal = (uNormalMatrix * vec4(vNormal, 0.0)).xyz;

	gl_Position = uProjectionMatrix * uViewMatrix * WorldPosition;
}
