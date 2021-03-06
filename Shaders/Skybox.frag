#version 330

layout (location = 0) out vec4 position;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 diffuse;
layout (location = 3) out vec4 emissive;
layout (location = 4) out vec4 occlusion;


in vec4 fragPos;
in vec3 fragNormal;
in mat3 toWorld;
in vec2 texCoords;
in float shininess;

uniform sampler2D colorTexture;
uniform sampler2D emissiveTexture;

void main() {
	position = fragPos;
	normal = vec4(0.0);//vec4(fragNormal,1.0);
	diffuse.rgb = texture(colorTexture, texCoords).rgb;
	diffuse.a = shininess;
	emissive = texture(colorTexture, texCoords);
	emissive.a = 0.0;
	vec4 emissiveColor = texture(emissiveTexture,texCoords);
	occlusion = vec4(emissiveColor.xyz * emissiveColor.w,1.0);
}