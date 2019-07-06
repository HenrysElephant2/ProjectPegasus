#include "PlayerMovementSystem.h"

PlayerMovementSystem::PlayerMovementSystem(MessageManager* messengerObject, ComponentManager<Transform> * transforms_in, ComponentManager<Player> * players_in): System(messengerObject)
{
	playerManager = players_in;
	transforms = transforms_in;
	frequency = SDL_GetPerformanceFrequency();
	// std::cout << this << std::endl;
}

void PlayerMovementSystem::update()
{	
	Uint64 currentTime = SDL_GetPerformanceCounter();
	frequency = SDL_GetPerformanceFrequency();
	float dt = ((currentTime - previousTime) / (float)frequency);
	previousTime = currentTime;

	Player * player = playerManager->getComponent(trackedPlayer);
	Transform * pLoc = transforms->getComponent(trackedPlayer);
	if(player && pLoc)
	{
		int i_move = w_Key - s_Key;
		int j_move = spacebar_Key - shift_Key;
		int k_move = d_Key - a_Key;

		glm::vec4 dPos = dt * 20.0f * glm::vec4(
			-i_move * sin(player->cameraYaw) + k_move * cos(player->cameraYaw),
			j_move,
			-k_move * sin(player->cameraYaw) - i_move * cos(player->cameraYaw),
			0.0
		);

		pLoc->position += dPos;
	}
}
void PlayerMovementSystem::receiveMessage(BasicMessage * message)
{
	// std::cout << "Received Message" << std::endl;
	Player * player = playerManager->getComponent(trackedPlayer);
	if(player)
	{
		KeyEvent * keyMessage;
		MouseEvent * mouseMessage;

		if((keyMessage = dynamic_cast<KeyEvent*>(message)) != NULL)
		{
			bool keyPress = keyMessage->event->type == SDL_KEYDOWN;
			switch(keyMessage->event->key.keysym.sym)
			{
				case SDLK_w: w_Key = keyPress; break;
				case SDLK_a: a_Key = keyPress; break;
				case SDLK_s: s_Key = keyPress; break;
				case SDLK_d: d_Key = keyPress; break;
				case SDLK_SPACE: spacebar_Key = keyPress; break;
				case SDLK_LSHIFT: shift_Key = keyPress; break;
				default: break;
			}
		}
		else if((mouseMessage = dynamic_cast<MouseEvent*>(message)) != NULL)
		{
			if( mouseMessage->event->type == SDL_MOUSEMOTION ) {
				int dx = mouseMessage->event->motion.xrel;
				int dy = mouseMessage->event->motion.yrel;
				player->cameraYaw -= dx * 3.1415926535 / 180.0 / 2.0;
				player->cameraPitch += dy * 3.1415926535 / 180.0 / 2.0;
				if( player->cameraPitch > 3.14 / 2.0 ) player->cameraPitch = 3.14 / 2.0;
				else if( player->cameraPitch < -3.14 / 2.0 ) player->cameraPitch = -3.14 / 2.0;
			}
		}
	}
	else {
		bool newPlayerFound = false;
		int i = 0;
		int count = playerManager->getSize();
		while(!newPlayerFound && i < count) {
			player = playerManager->getComponent(i);
			
			if(player) {
				trackedPlayer = i;
				newPlayerFound = true;
			}
			i++;
		}
	}
}