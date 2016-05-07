
#version 330

in vec3 vPosition;
in vec3 vColor;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 fColor;
out vec4 fPosition;


void main()
{
	gl_Position = fPosition = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(vPosition.xyz, 1);
	fColor = vColor;
}
