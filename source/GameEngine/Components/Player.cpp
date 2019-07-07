#include "Components/Player.h"

void Player::readFromXML(XmlElement * element) {
	element->queryFloatAttribute("cameraOffset",&cameraOffset);
	element->queryFloatAttribute("cameraYaw",&cameraYaw);
	element->queryFloatAttribute("cameraPitch",&cameraPitch);
}