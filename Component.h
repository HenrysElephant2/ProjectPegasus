
#ifndef COMPONENT_H
#define COMPONENT_H

#include <glm/glm.hpp>


struct Component {
	int ownerID; // entityID for the entity that owns this component
};


struct Transform:Component {
	glm::vec3 position;
	//add rotation and scale, not sure best way to store these

};

#endif