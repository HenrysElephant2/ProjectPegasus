
#ifndef COMPONENT_H
#define COMPONENT_H

#define SHADOW_MAP_DIMENSION 1024

#define GL_GLEXT_PROTOTYPES 1
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "MessageManager.h"
#include <SDL2/SDL_opengl.h>
#include "Material.h"
#include <iostream>

struct Component {
	int ownerID; // entityID for the entity that owns this component
	Component(int owner){ownerID = owner;}
	Component(){}
};


struct Transform:Component {
	glm::vec4 position = glm::vec4(0.0,0.0,0.0,1.0);
	glm::vec3 orientation = glm::vec3(0.0,0.0,0.0);
	float scale = 1.0;
	Transform(glm::vec4& position_in, glm::vec3& orientation_in, float scale_in, int ownerID) : Component(ownerID)
	{
		position = position_in;
		orientation = orientation_in;
		scale = scale_in;
	}
	Transform(){}
};

struct Renderable:Component {
	GLuint VBO;
	GLuint IBO;
	int numVertices;
	int program; // index into shadermanager
	Material material;
	Renderable(GLuint VBO_in, GLuint IBO_in, int vertex_Count, int program_in, Material & material_in, int ownerID):Component(ownerID)
	{
		VBO = VBO_in;
		IBO = IBO_in;
		numVertices = vertex_Count;
		program = program_in;
		material = material_in;
	}
	Renderable(){}
};

struct Physics:Component {
	glm::vec3 velocity;
	float mass;
	bool freeze; // add this?
	bool onGround; // useful for player movement?
};

struct Collider:Component {

};

struct Trigger:Component {
	float radius;
	BasicMessage m;
};

struct Grapple:Component {

};

struct Player:Component {
	float cameraOffset = 1.0; //distance from transform of player to camera
	float cameraYaw = 0;
	float cameraPitch = 0;
	//float cameraRoll; // add this later if we want
	Player(int ownerID):Component(ownerID){}
};

struct Light:Component {
	glm::vec3 location = glm::vec3(0.0,0.0,0.0);
	glm::vec3 diffuse = glm::vec3(1.0,0.0,1.0);
	glm::vec3 specular = glm::vec3(1.0,0.0,1.0);
	float linearAttenuation = 0.0;
	float quadraticAttenuation = 0.0;
	GLuint shadowMapTextures[6] = {0,0,0,0,0,0}; // A cube of textures representing this light's shadow map: +x, -x, +y, -y, +z, -z
	GLuint shadowCubeMap = 0;

	Light(){
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
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		if( shadowCubeMap == 0 ) {
			glGenTextures(1, &shadowCubeMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);
			for( int i=0; i<6; i++ ) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_DEPTH_COMPONENT24, SHADOW_MAP_DIMENSION, SHADOW_MAP_DIMENSION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			}
		    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
	}
	Light(glm::vec3 &location_in, glm::vec3 &diffuse_in, glm::vec3 &specular_in, float linearAtt_in, float quadraticAtt_in, int ownerID):Component(ownerID)
	{
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
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		
		if( shadowCubeMap == 0 ) {
			glGenTextures(1, &shadowCubeMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);
			for( int i=0; i<6; i++ ) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_DEPTH_COMPONENT24, SHADOW_MAP_DIMENSION, SHADOW_MAP_DIMENSION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
	}
};

#endif