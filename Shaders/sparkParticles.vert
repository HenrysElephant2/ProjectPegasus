#version 330 core

layout(location = 0) in vec3 Vertex;
layout(location = 1) in vec4 Color;
layout(location = 2) in vec3 Normal; // To be used as starting x and z velocity here 
layout(location = 5) in vec2 Texture; // To be used as ground time and decay time

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform float time;

out vec4 col;
out float t;
out float decayT;

void main() {
	vec3 pos = Vertex + min(Texture.x, time) * (Normal + vec3(0.0,-7.0*time,0.0));
	if( pos.y < 0.0 ) pos.y = 0.0;
	col = Color;
	// col = vec4(Texture, 0.2, 1.0);
	t = time;
	decayT = Texture.y;
	gl_Position = Projection * View * Model * vec4(pos,1.0);
}