#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D uContainerTex;

void main()
{
	FragColor = texture(uContainerTex, TexCoord);
}