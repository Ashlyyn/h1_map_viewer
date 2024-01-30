#version 330 core
layout (location = 0) in vec2 aVertex;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 uModel;
uniform mat4 uProjection;

void main()
{
    gl_Position =  uProjection * uModel * vec4(aVertex, 0.0, 1.0);
    TexCoords = aTexCoords;
}