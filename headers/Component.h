
#ifndef COMPONENT_H
#define COMPONENT_H

#include <glm/glm.hpp>
#include "MessageManager.h"
#include <OpenGL/gl3.h>
#include "Material.h"

struct Component {
	int ownerID; // entityID for the entity that owns this component
	Component(int owner){ownerID = owner;}
	Component(){}
};


struct Transform:Component {
	glm::vec4 position;
	glm::vec3 orientation;
	float scale;
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

#endif