#include "Components/Transform.h"

void Transform::readFromXML(XmlElement * element) {

	// fill in position data
	element->queryFloatAttribute("positionX",&position.x);
	element->queryFloatAttribute("positionY",&position.y);
	element->queryFloatAttribute("positionZ",&position.z);
	element->queryFloatAttribute("positionW",&position.w);

	//fill in orientation data
	element->queryFloatAttribute("orientationX",&orientation.x);
	element->queryFloatAttribute("orientationY",&orientation.y);
	element->queryFloatAttribute("orientationZ",&orientation.z);

	element->queryFloatAttribute("scale",&scale);
}