#version 330

uniform sampler2D baseColor;
uniform sampler2D bloomColor;

in vec2 texCoords;

out vec4 FragColor;


void main(){
	FragColor = vec4((texture(baseColor, texCoords) + texture(bloomColor, texCoords)).rgb,1.0);
}
