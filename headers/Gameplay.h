#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <SDL2/SDL.h>
#include <typeinfo>
#include "EntityManager.h"
#include "LevelLoader.h"
#include "ECSEngine.h"
#include "State.h"
#include "RenderSystem.h"
#include "ShaderManager.h"
#include "MessageManager.h"

class Gameplay:public State{
private:
	ECSEngine* engine;
	RenderSystem * render_Reference; // only to be used for reshaping window
	MessageManager * messenger;
	
public:
	Gameplay(int width, int height,std::string &level_directory);
	~Gameplay();
	void update();
	void keyEvent(SDL_Event * e);
	void mouseEvent(SDL_Event * e);
	void reshape(int width, int height);
};

#endif