#version 330 core

in vec4 col;
in float t;
in float decayT;

// layout (location = 0) out vec4 position;
// layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 diffuse;
layout (location = 3) out vec4 emissive;

void main() {
	if( t < decayT ) {
		emissive = vec4(vec3(col),1.0);
		// diffuse = vec4(1.0,1.0,1.0,1.0);
	}
}