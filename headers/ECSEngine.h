
#ifndef ECS_ENGINE
#define ECS_ENGINE

#include "MessageManager.h"
#include "System.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "Component.h"
#include "ShaderManager.h"
#include "RenderSystem.h"
#include "PlayerMovementSystem.h"
#include "AnimationSystem.h"

class ECSEngine {
private:

	static ECSEngine * object;

	

	MessageManager messenger;
	EntityManager entities;
	ShaderManager * shaders;
	RenderSystem rendersystemObject;
	PlayerMovementSystem playerMovementSystemObject;
	AnimationSystem animationHandler;
	ComponentManager<Transform> transformManager;
	ComponentManager<Renderable> renderableManager;
	ComponentManager<Player> playerManager;
	ComponentManager<Light> lightManager;
	ComponentManager<SkinnedRenderable> skinnedManager;
	ComponentManager<ParticleSystem> particleManager;

public:
	
	ECSEngine();
	//~ECSEngine(){}
	void update();
	void spawnMessage(BasicMessage* message);
	int addEntity();
	void addTransform(int entityID, Transform &comp);
	void addRenderable(int entityID, Renderable &comp);
	void addPlayer(int entityID, Player &comp);
	void addLight(int entityID, Light &comp);
	void addSkinnedRenderable(int entityID, SkinnedRenderable & comp);
	void addParticleSystem(int entityID, ParticleSystem &comp);
	RenderSystem* getRenderSystem();

	static ECSEngine * createECS();


};

#endif