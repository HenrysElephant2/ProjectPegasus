#version 330

layout(location = 0) in vec4 Vertex;
layout(location = 5) in vec2 Texture;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 texCoords;

void main() {
	texCoords = Texture;
	gl_Position = Projection * View * Model * Vertex;
}