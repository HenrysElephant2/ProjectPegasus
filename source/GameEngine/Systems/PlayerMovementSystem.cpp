#include "PlayerMovementSystem.h"
void PlayerMovementSystem::update()
{
	Uint64 dt = (SDL_GetPerformanceCounter() - previousTime)/frequency;
	Player * player = playerManager->getComponent(trackedPlayer);
	if(player)
	{
		// if moving and angle of camera is not 0, turn players orientation to point in direction that the camera is pointed, and adjust camera to have a new offset that places the camera at 
		// the same location in space. next multiply camera angle by ~.9, so that over time as the player moves the camera will move?

		// nah scrap this just turn player's orientation to point towards camera is pointing and move in that direction. can work on turning player more slowly later. 
		// perhaps by turning the player only a little bit at a time and letting the character move faster the closer camera angle is to 0
	}
}
void PlayerMovementSystem::receiveMessage(BasicMessage * message)
{
	KeyEvent * keyMessage;
	MouseEvent * mouseMessage;

	if((keyMessage = dynamic_cast<KeyEvent*>(message)) != NULL)
	{
		bool keyPress = keyMessage->event->type == SDL_KEYDOWN;
		switch(keyMessage->event->text.text[ 0 ])
		{
			case 'w':
				w_Key = keyPress;
			case 'a':
				a_Key = keyPress;
			case 's':
				s_Key = keyPress;
			case 'd':
				d_Key = keyPress;
			case ' ':
				spacebar_Key = keyPress;
		}
	}
	else if((mouseMessage = dynamic_cast<MouseEvent*>(message)) != NULL)
	{

	}
}