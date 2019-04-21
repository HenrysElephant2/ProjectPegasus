#version 330 core

layout (location = 0) out uint FragColor;
layout (location = 1) out vec4 TempColor;

in vec4 loc;
in vec4 norm;
in vec4 lightSpaceLoc[6];

uniform vec3 LightLoc;
uniform int LightIndex;
uniform usampler2D currentTex;
uniform sampler2D ShadowMaps[6];
uniform vec2 WindowSize;

const float near_plane = 0.05;
const float far_plane  = 20.0;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // Back to NDC 
	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main() {
	vec3 lightDir = loc.xyz - LightLoc;
	float lit = 0.0;
	if( dot(norm.xyz, lightDir) < 0.0 ) {
		int useMap = 0;
		if(abs(lightDir.x) > max(abs(lightDir.y),abs(lightDir.z))) {
			if( lightDir.x < 0 ) {
				useMap = 1;
			}
		}
		else if(abs(lightDir.y) > abs(lightDir.z)) {
			if( lightDir.y < 0 ) {
				useMap = 3;
			}
			else {
				useMap = 2;
			}
		}
		else{
			if( lightDir.z < 0 ) {
				useMap = 5;
			}
			else {
				useMap = 4;
			}
		}
		// useMap = 0;

		vec3 projloc = lightSpaceLoc[useMap].xyz / lightSpaceLoc[useMap].w;
		// vec3 projloc = lightSpaceLoc[useMap].xyz;
		// transform to [0,1] range
		projloc = projloc * 0.5 + 0.5;
		// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
		float closestDepth = texture(ShadowMaps[useMap], projloc.xy).r; 
		

		// vec3 projloc = gl_FragCoord.xyz; // / gl_FragCoord.w;
		// vec2 projtex = vec2(projloc.x/WindowSize.x, projloc.y/WindowSize.y);
		// float closestDepth = texture(ShadowMaps[useMap], projtex).r; 

		// get depth of current fragment from light's perspective
		float currentDepth = projloc.z;
		// check whether current frag pos is in shadow
		float bias = 0.0005;
		lit = currentDepth - bias < closestDepth ? 1.0 : 0.0;
		// if( projloc.x > 1 || projloc.x < 0 || projloc.y > 1 || projloc.y < 0 ) {
		// 	lit = 0.0;
		// }
	}

	vec3 testLoc = gl_FragCoord.xyz; // / gl_FragCoord.w;
	vec2 texCoords = vec2(testLoc.x/WindowSize.x, testLoc.y/WindowSize.y);
	uint current = texture(currentTex, texCoords.xy).r;

	FragColor = current | (uint(lit) << (LightIndex));
	// FragColor = vec4(lit);
	// TempColor = vec4(vec3(LinearizeDepth(closestDepth)),1.0);
	// FragColor = current | (1u << LightIndex);
}
