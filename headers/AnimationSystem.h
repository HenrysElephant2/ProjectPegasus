#ifndef ANIMATION_SYSTEM_H
#define ANIMATION_SYSTEM_H

#include "EntityManager.h"
#include "MessageManager.h"
#include "ComponentManager.h"
#include "Components/SkinnedRenderable.h"

class AnimationSystem : System {
private:
	ComponentManager<SkinnedRenderable>* rigs;
	Uint64 previousTime;
	Uint64 frequency;


public:
	AnimationSystem():System()
	{
		messenger->subscribe(this, ANIMATION_MESSAGE);
		SkinnedRenderable r = SkinnedRenderable();
		rigs = EntityManager::getEntityManager()->getComponentManager(r);
		frequency = SDL_GetPerformanceFrequency();
		previousTime = SDL_GetPerformanceCounter();
	}
	void update();
	void receiveMessage(BasicMessage * message);
};

#endif