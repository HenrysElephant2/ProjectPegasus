#version 330 core

layout(location = 0) in vec3 Vertex;
layout(location = 1) in vec4 Color;
layout(location = 2) in vec3 Normal; // To be used as starting x and z velocity here 

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform float time;

out vec4 col;

void main() {
	vec3 pos = Vertex + time * (Normal + vec3(0.0,-7.0*time,0.0));
	col = Color;
	gl_Position = Projection * View * Model * vec4(pos,1.0);
}