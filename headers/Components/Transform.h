#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Component.h"

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

	void readFromXML(XmlElement * element);
};

#endif