
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
	EntityManager * entities = EntityManager::getEntityManager();
	ShaderManager * shaders;
	RenderSystem rendersystemObject;
	PlayerMovementSystem playerMovementSystemObject;
	AnimationSystem animationHandler;
	// ComponentManager<Transform> transformManager;
	// ComponentManager<Renderable> renderableManager;
	// ComponentManager<Player> playerManager;
	// ComponentManager<Light> lightManager;
	// ComponentManager<SkinnedRenderable> skinnedManager;
	// ComponentManager<ParticleSystem> particleManager;

public:
	
	ECSEngine();
	//~ECSEngine(){}
	void update();
	void spawnMessage(BasicMessage* message);
	int addEntity();
	// void addTransform(int entityID, Transform &comp);
	// void addRenderable(int entityID, Renderable &comp);
	// void addPlayer(int entityID, Player &comp);
	// void addLight(int entityID, Light &comp);
	// void addSkinnedRenderable(int entityID, SkinnedRenderable & comp);
	// void addParticleSystem(int entityID, ParticleSystem &comp);
	RenderSystem* getRenderSystem();

	// ComponentManager<Transform> * getTransformManager();
	// ComponentManager<Renderable> * getRenderableManager();
	// ComponentManager<Player> * getPlayerManager();
	// ComponentManager<Light> * getLightManager();
	// ComponentManager<SkinnedRenderable> * getSkinnedRenderableManager();
	// ComponentManager<ParticleSystem> * getParticleSystemManager();

	static ECSEngine * createECS();


};

#endif