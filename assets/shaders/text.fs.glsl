#version 330 core
in vec2 GlyphTexCoords;

uniform sampler2D uTex;
uniform vec3 uTextColor;
uniform vec4 uAtlasCoord;
uniform mat4 uModel;

out vec4 FragColor;

void main() {
	FragColor = vec4(1.0, 1.0, 1.0, texture2D(uTex, uAtlasCoord.xy + GlyphTexCoords * uAtlasCoord.zw).r) * vec4(uTextColor, 1.0);
}