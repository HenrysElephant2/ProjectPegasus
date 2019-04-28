#ifndef STATE_H
#define STATE_H

#include <SDL2/SDL.h>
#include "ShaderManager.h"

class State {
protected:
	ShaderManager * shaders;
public:
	State(ShaderManager * shaders_in)
	{
		shaders = shaders_in;
	}
	virtual ~State() {}
	virtual void update() = 0;
	virtual void keyEvent(SDL_Event * e) = 0;
	virtual void mouseEvent(SDL_Event * e) = 0;
	virtual void reshape(int width, int height) = 0;

};

#endif