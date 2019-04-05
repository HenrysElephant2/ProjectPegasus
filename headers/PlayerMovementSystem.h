#ifndef PLAYERMOVEMENT_H
#define PLAYERMOVEMENT_H

#include "Component.h"
#include "ComponentManager.h"
#include "System.h"

class PlayerMovementSystem:System {
private:
	int trackedPlayer = 0; // only moves one player, which is the player with entityID trackedPlayer
	ComponentManager<Player> players;
	ComponentManager<Transform> transforms;

public:
	PlayerMovementSystem(MessageManager* messengerObject, ComponentManager<Transform> transforms_in, ComponentManager<Player> players_in)
	 : System(messengerObject)
	{
		players = players_in;
		transforms = transforms_in;
	}

	void update();
	void receiveMessage(BasicMessage *message);

};

#endif