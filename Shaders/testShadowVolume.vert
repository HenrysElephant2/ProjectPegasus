#version 330 core
layout (location = 0) in vec4 Vertex;
layout (location = 2) in vec4 Normal;

out vec4 loc;
out vec4 norm;
out vec4 lightSpaceLoc[6];

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 LightViews[6];
uniform mat4 LightSpaceMatrix;

void main() {
	loc = Model * Vertex;
	norm = Normal;
	for( int i=0; i<6; i++ ) {
		lightSpaceLoc[i] = LightSpaceMatrix * LightViews[i] * loc;
	}
	// gl_Position = LightSpaceMatrix * LightViews[0] * Model * Vertex;
	gl_Position = Projection * View * Model * Vertex;
}