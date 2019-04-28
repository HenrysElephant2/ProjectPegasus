#ifndef ANIMATION_SYSTEM_H
#define ANIMATION_SYSTEM_H

#include "MessageManager.h"
#include "ComponentManager.h"
#include "Component.h"

class AnimationSystem : System {
private:
	ComponentManager<SkinnedRenderable>* rigs;
	Uint64 previousTime;
	Uint64 frequency;


public:
	AnimationSystem(MessageManager * m, ComponentManager<SkinnedRenderable>* rigs_in):System(m)
	{
		m->subscribe(this, ANIMATION_MESSAGE);
		rigs = rigs_in;
		frequency = SDL_GetPerformanceFrequency();
		previousTime = SDL_GetPerformanceCounter();
	}
	AnimationSystem():System(NULL){}
	void update();
	void receiveMessage(BasicMessage * message);
};

#endif