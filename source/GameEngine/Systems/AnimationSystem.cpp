#include "AnimationSystem.h"

void AnimationSystem::update()
{
	Uint64 currentTime = SDL_GetPerformanceCounter();
	float dt = (currentTime - previousTime)/((double)frequency);
	previousTime = currentTime;
	// std::cout << "Updating animations" << std::endl;
	int count = rigs->getSize();
	for(int i = 0; i < count; i++)
	{
		SkinnedRenderable * r = rigs->getComponent(i);
		if(r)
		{
			r->bones.updateAnimation(dt);
			//std::cout << "updated rig" << std::endl;
		}
	}
}


void AnimationSystem::receiveMessage(BasicMessage * message)
{

}