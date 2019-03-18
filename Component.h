
#ifndef COMPONENT_H
#define COMPONENT_H

#include <glm/glm.hpp>
#include "MessageManager.h"
#include <OpenGL/gl3.h>
#include "Material.h"

struct Component {
	int ownerID; // entityID for the entity that owns this component
	Component(int owner){ownerID = owner;}
};


struct Transform:Component {
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 up;
	float scale;
	Transform(glm::vec3& position_in, glm::vec3& forward_in, glm::vec3& up_in, float scale_in, int ownerID) : Component(ownerID)
	{
		position = position_in;
		forward = forward_in;
		up = up_in;
		scale = scale_in;
	}
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
};

struct Physics:Component {
	glm::vec3 velocity;
	float mass;
	bool freeze; // add this?
};

struct Collider:Component {

};

struct Trigger:Component {
	float radius;
	BasicMessage m;
};

struct Grapple:Component {

};

#endif