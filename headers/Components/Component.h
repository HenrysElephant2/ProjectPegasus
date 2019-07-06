
#ifndef COMPONENT_H
#define COMPONENT_H

#define SHADOW_MAP_DIMENSION 2048

#define GL_GLEXT_PROTOTYPES 1
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "MessageManager.h"
#include <SDL2/SDL_opengl.h>
#include "Material.h"
#include "Animation.h"
#include "XMLParserAdapter.h"
#include <iostream>

struct Component {
	int ownerID; // entityID for the entity that owns this component
	Component(int owner){ownerID = owner;}
	Component(){}
	virtual void readFromXML(XmlElement * element) = 0;
};

// struct Physics:Component {
// 	glm::vec3 velocity;
// 	float mass;
// 	bool freeze; // add this?
// 	bool onGround; // useful for player movement?
// };

// struct Collider:Component {

// };

// struct Trigger:Component {
// 	float radius;
// 	BasicMessage m;
// };

// struct Grapple:Component {

// };

#endif