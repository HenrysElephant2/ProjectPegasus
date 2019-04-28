#version 330 core

layout (location = 0) out uint FragColor;
layout (location = 1) out vec4 TempColor;

in vec2 texCoords;

uniform mat4 LightViews[6];
uniform mat4 LightSpaceMatrix;

uniform vec3 LightLoc;
uniform int LightIndex;

uniform usampler2D currentTex;
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;

uniform sampler2D ShadowMap0;
uniform sampler2D ShadowMap1;
uniform sampler2D ShadowMap2;
uniform sampler2D ShadowMap3;
uniform sampler2D ShadowMap4;
uniform sampler2D ShadowMap5;

const float near_plane = 0.05;
const float far_plane  = 20.0;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // Back to NDC 
	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main() {
	vec3 loc = texture(positionTexture, texCoords).rgb;
	vec3 norm = normalize(texture(normalTexture, texCoords).rgb);

	vec3 lightDir = loc - LightLoc;
	float lit = 0.0;

	if( dot(norm, lightDir) < 0.0 ) {
		vec3 projloc;
		float closestDepth;
		vec4 lightSpaceLoc;

		if(abs(lightDir.x) > max(abs(lightDir.y),abs(lightDir.z))) {
			if( lightDir.x > 0 ) {
				lightSpaceLoc = LightSpaceMatrix * LightViews[0] * vec4(loc,1.0);
				projloc = (lightSpaceLoc.xyz / lightSpaceLoc.w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap0, projloc.xy).r; 
			}
			else {
				lightSpaceLoc = LightSpaceMatrix * LightViews[1] * vec4(loc,1.0);
				projloc = (lightSpaceLoc.xyz / lightSpaceLoc.w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap1, projloc.xy).r; 
			}
		}
		else if(abs(lightDir.y) > abs(lightDir.z)) {
			if( lightDir.y > 0 ) {
				lightSpaceLoc = LightSpaceMatrix * LightViews[2] * vec4(loc,1.0);
				projloc = (lightSpaceLoc.xyz / lightSpaceLoc.w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap2, projloc.xy).r; 
			}
			else {
				lightSpaceLoc = LightSpaceMatrix * LightViews[3] * vec4(loc,1.0);
				projloc = (lightSpaceLoc.xyz / lightSpaceLoc.w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap3, projloc.xy).r; 
			}
		}
		else{
			if( lightDir.z > 0 ) {
				lightSpaceLoc = LightSpaceMatrix * LightViews[4] * vec4(loc,1.0);
				projloc = (lightSpaceLoc.xyz / lightSpaceLoc.w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap4, projloc.xy).r; 
			}
			else {
				lightSpaceLoc = LightSpaceMatrix * LightViews[5] * vec4(loc,1.0);
				projloc = (lightSpaceLoc.xyz / lightSpaceLoc.w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap5, projloc.xy).r; 
			}
		}

		// get depth of current fragment from light's perspective
		float currentDepth = projloc.z;
		// check whether current frag pos is in shadow
		float bias = 0.0005;
		lit = currentDepth - bias < closestDepth ? 1.0 : 0.0;
	}

	uint current = texture(currentTex, texCoords).r;

	// Set corresponding bit of integer texture to reflect shadow value
	FragColor = current | (uint(lit) << (LightIndex));
}
