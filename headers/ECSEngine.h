
#ifndef ECS_ENGINE
#define ECS_ENGINE

#include "MessageManager.h"
#include "System.h"
#include "ComponentManager.h"
#include "EntityManager.h"

#include "Components/Transform.h"
#include "Components/Renderable.h"
#include "Components/Player.h"
#include "Components/Light.h"
#include "Components/SkinnedRenderable.h"
#include "Components/ParticleSystem.h"

#include "ShaderManager.h"
#include "RenderSystem.h"
#include "PlayerMovementSystem.h"
#include "AnimationSystem.h"

class ECSEngine {
private:

	static ECSEngine * object;

	

	MessageManager messenger;
	EntityManager * entities;
	ShaderManager * shaders;
	RenderSystem rendersystemObject;
	PlayerMovementSystem playerMovementSystemObject;
	AnimationSystem animationHandler;

public:
	
	ECSEngine();
	void update();
	void spawnMessage(BasicMessage* message);
	int addEntity();
	RenderSystem* getRenderSystem();

	static ECSEngine * createECS();


};

#endif