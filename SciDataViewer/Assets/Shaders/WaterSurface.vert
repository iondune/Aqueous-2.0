
#version 330

in vec2 vPosition;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform vec3 uCameraPosition;

out vec3 vNormal;


void main()
{
	float x = vPosition.x;// + uCameraPosition.x;
	float y = vPosition.y;// + uCameraPosition.z;
	float Height = sin(x) + cos(y);
	vec4 WorldPosition = uModelMatrix * vec4(
		vPosition.x,// - 64.0 + uCameraPosition.x,
		Height,
		vPosition.y,// - 64.0 + uCameraPosition.z,
		1.0);

	vNormal = vec3(0.0, 1.0, 0.0);

	gl_Position = uProjectionMatrix * uViewMatrix * WorldPosition;
}
