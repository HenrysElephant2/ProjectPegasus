#version 330 core

layout (location = 0) out vec4 FragColor;

in vec4 loc;
in vec4 lightSpaceLoc[6];

uniform vec3 LightLoc;
uniform int LightIndex;
uniform sampler2D ShadowMaps[6];

void main() {
	vec3 testVec = loc.xyz - LightLoc;
	// float maxVal = max(abs(testVec.x), max(abs(testVec.y), abs(testVec.z)));
	// int useMap = (maxVal == abs(testVec.y))*2 + (maxVal == abs(testVec.z))*4
	int useMap = 0;
	if(abs(testVec.x) > max(abs(testVec.y),abs(testVec.z))) {
		if( testVec.x < 0 ) {
			useMap = 1;
		}
	}
	else if(abs(testVec.y) > abs(testVec.z)) {
		if( testVec.y < 0 ) {
			useMap = 3;
		}
		else {
			useMap = 2;
		}
	}
	else{
		if( testVec.z < 0 ) {
			useMap = 5;
		}
		else {
			useMap = 4;
		}
	}
	vec3 projloc = lightSpaceLoc[useMap].xyz / lightSpaceLoc[useMap].w;
    // transform to [0,1] range
	projloc = projloc * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(ShadowMaps[useMap], projloc.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projloc.z;
    // check whether current frag pos is in shadow
    int shadow = currentDepth > closestDepth ? 1 : 0;

	FragColor = current & (uint(1) << (LightIndex - 1));
}
