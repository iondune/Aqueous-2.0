#version 150

in vec3 fColor;

out vec4 outColor;


void main()
{
	outColor = texture(uTexture, fTexCoords) * fAlpha;
}
