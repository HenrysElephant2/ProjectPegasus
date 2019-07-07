#include "Components/Light.h"

void Light::readFromXML(XmlElement * element) {
	element->queryFloatAttribute("linearAttenuation",&linearAttenuation);
	element->queryFloatAttribute("quadraticAttenuation",&quadraticAttenuation);

	element->queryFloatAttribute("locationOffsetX",&location.x);
	element->queryFloatAttribute("locationOffsetY",&location.y);
	element->queryFloatAttribute("locationOffsetZ",&location.z);

	element->queryFloatAttribute("diffuseRed",&diffuse.x);
	element->queryFloatAttribute("diffuseGreen",&diffuse.y);
	element->queryFloatAttribute("diffuseBlue",&diffuse.z);

	element->queryFloatAttribute("specularRed",&specular.x);
	element->queryFloatAttribute("specularGreen",&specular.y);
	element->queryFloatAttribute("specularBlue",&specular.z);

}