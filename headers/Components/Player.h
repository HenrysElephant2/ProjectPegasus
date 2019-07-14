#ifndef PLAYER_H
#define PLAYER_H

#include "Component.h"

struct Player:Component {
	float cameraOffset = 1.0; //distance from transform of player to camera
	float cameraYaw = 0;
	float cameraPitch = 0;
	//float cameraRoll; // add this later if we want
	Player(int ownerID):Component(ownerID){}
	Player():Component(-1){}

	void readFromXML(XmlElement * element);
};

#endif