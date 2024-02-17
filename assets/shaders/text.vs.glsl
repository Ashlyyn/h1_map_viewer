#version 330 core
layout (location = 0) in vec2 aVertex;
layout (location = 1) in vec2 aTexCoords;

out vec2 GlyphTexCoords;

uniform mat4 uModel;
uniform mat4 uOrthoProjection;
uniform vec4 uAtlasCoord;

void main() {
    gl_Position = uOrthoProjection * uModel * vec4(aVertex, 0.0, 1.0);
    GlyphTexCoords = aTexCoords;
}