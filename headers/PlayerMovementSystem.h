#ifndef PLAYERMOVEMENT_H
#define PLAYERMOVEMENT_H

#include <SDL2/SDL.h>
#include "EntityManager.h"
#include "Components/Transform.h"
#include "Components/Player.h"
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
	bool w_Key, a_Key, s_Key, d_Key, spacebar_Key, shift_Key;

public:
	PlayerMovementSystem():System(NULL) {}
	PlayerMovementSystem(MessageManager* messengerObject);

	void update();
	void receiveMessage(BasicMessage * message);
};

#endif