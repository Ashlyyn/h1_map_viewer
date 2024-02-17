#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uPerspectiveProjection;

void main()
{
	gl_Position = uPerspectiveProjection * uView * uModel * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}