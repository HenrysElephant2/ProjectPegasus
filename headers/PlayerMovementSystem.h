#ifndef PLAYERMOVEMENT_H
#define PLAYERMOVEMENT_H

#include <SDL2/SDL.h>
#include "Component.h"
#include "ComponentManager.h"
#include "System.h"

class PlayerMovementSystem:System {
private:
	int trackedPlayer = 0; // only moves one player, which is the player with entityID trackedPlayer
	ComponentManager<Player> * playerManager;
	ComponentManager<Transform> * transforms;

	Uint64 previousTime = 0;
	Uint64 frequency;

	// keep track of what keys are down
	bool w_Key, a_Key, s_Key, d_Key, spacebar_Key;

public:
	PlayerMovementSystem(MessageManager* messengerObject, ComponentManager<Transform> * transforms_in, ComponentManager<Player> * players_in)
	 : System(messengerObject)
	{
		playerManager = players_in;
		transforms = transforms_in;
		messengerObject->subscribe(this, KEY_EVENT);
		messengerObject->subscribe(this, MOUSE_EVENT);
		frequency = SDL_GetPerformanceFrequency();
	}

	void update();
	void receiveMessage(BasicMessage * message);
};

#endif