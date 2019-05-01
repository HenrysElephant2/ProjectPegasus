#version 330 core

in vec4 col;

// layout (location = 0) out vec4 position;
// layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 diffuse;
layout (location = 3) out vec4 emissive;

void main() {
	emissive = col;
	// diffuse = vec4(1.0,1.0,1.0,1.0);
}