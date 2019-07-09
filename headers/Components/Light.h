 #ifndef LIGHT_H
#define LIGHT_H


#include "Component.h"

struct Light:Component {
	bool directional = false;
	glm::vec3 location = glm::vec3(0.0,0.0,0.0);
	glm::vec3 diffuse = glm::vec3(1.0,0.0,1.0);
	glm::vec3 specular = glm::vec3(1.0,0.0,1.0);
	glm::vec3 direction = glm::vec3(-1.0,-1.0,-1.0);
	float linearAttenuation = 0.0;
	float quadraticAttenuation = 0.0;

	GLuint shadowMapTextures[6] = {0,0,0,0,0,0}; // A cube of textures representing this light's shadow map: +x, -x, +y, -y, +z, -z
	GLuint combinedShadowMapTexture = 0;

	Light(){
		if( shadowMapTextures[0] == 0 )  {
			for( unsigned int i=0; i<6; i++ ) {
				glGenTextures(1, &shadowMapTextures[i]);
				glBindTexture(GL_TEXTURE_2D, shadowMapTextures[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_DIMENSION, SHADOW_MAP_DIMENSION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			glGenTextures(1, &combinedShadowMapTexture);
			glBindTexture(GL_TEXTURE_2D, combinedShadowMapTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 6*SHADOW_MAP_DIMENSION, SHADOW_MAP_DIMENSION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	Light(bool directional_in, glm::vec3 &location_in, glm::vec3 &diffuse_in, glm::vec3 &specular_in, float linearAtt_in, float quadraticAtt_in, int ownerID):Component(ownerID) {
		directional = directional_in;
		location = location_in;
		diffuse = diffuse_in;
		specular = specular_in;
		linearAttenuation = linearAtt_in;
		quadraticAttenuation = quadraticAtt_in;
		if( shadowMapTextures[0] == 0 ) {
			for( unsigned int i=0; i<6; i++ ) {
				glGenTextures(1, &shadowMapTextures[i]);
				glBindTexture(GL_TEXTURE_2D, shadowMapTextures[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_DIMENSION, SHADOW_MAP_DIMENSION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			glGenTextures(1, &combinedShadowMapTexture);
			glBindTexture(GL_TEXTURE_2D, combinedShadowMapTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 6*SHADOW_MAP_DIMENSION, SHADOW_MAP_DIMENSION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void readFromXML(XmlElement * element);
};

#endif