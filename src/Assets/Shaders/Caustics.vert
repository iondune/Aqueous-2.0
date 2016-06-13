#version 330 core

in vec3 vPosition;
in vec3 vTexCoords;


uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uCaustVPMatrix;
uniform mat4 uProjectionMatrix;

out vec4 caustCoords;
out vec4 fragPos;

void main()
{
   vec4 wPos = uModelMatrix * vec4(vPosition, 1.0);
	gl_Position = uProjectionMatrix * uViewMatrix * wPos;
   caustCoords =  uCaustVPMatrix*wPos;
   fragPos = uViewMatrix * wPos;

}
