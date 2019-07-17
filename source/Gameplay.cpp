#include "Gameplay.h"

Gameplay::Gameplay(int width, int height,std::string &level_directory):State()
{
	messenger = MessageManager::getMessageManager();

	// load all ComponentManagers into the EntityManager here, or call a different class to do it.
	EntityManager * entityManager = EntityManager::getEntityManager();

	Transform t = Transform();
	const std::type_info & type = typeid(t);
	entityManager->addComponentManager(type,new ComponentManager<Transform>());

	Renderable r = Renderable();
	const std::type_info & type1 = typeid(r);
	entityManager->addComponentManager(type1,new ComponentManager<Renderable>());

	Player p = Player();
	const std::type_info & type2 = typeid(p);
	entityManager->addComponentManager(type2,new ComponentManager<Player>());

	Light l = Light();
	const std::type_info & type3 = typeid(l);
	entityManager->addComponentManager(type3,new ComponentManager<Light>());

	SkinnedRenderable sr = SkinnedRenderable();
	const std::type_info & type4 = typeid(sr);
	entityManager->addComponentManager(type4,new ComponentManager<SkinnedRenderable>());

	ParticleSystem ps = ParticleSystem();
	const std::type_info & type5 = typeid(ps);
	entityManager->addComponentManager(type5,new ComponentManager<ParticleSystem>());

	engine = new ECSEngine();
	std::cout << "Creating ECS" << std::endl;

	render_Reference = new RenderSystem();
	PlayerMovementSystem * movement = new PlayerMovementSystem();
	AnimationSystem * animation = new AnimationSystem();
	
	

	std::cout << "Initialized all Systems" << std::endl;

	engine->addSystem((System *)movement);
	engine->addSystem((System *)animation);
	engine->addSystem((System *)render_Reference);

	std::cout << "Registered systems into ECS" << std::endl;

	render_Reference->reshape(width,height);

	std::cout << "Reshaped window" << std::endl;

	std::cout << "Starting to load level" << std::endl;
	LevelLoader level = LevelLoader();
	level.openLevel(level_directory,engine);
	std::cout << "Successfully created ECS" << std::endl;

	glClearColor( 0.f, 0.f, 0.f, 1.f );
}

Gameplay::~Gameplay()
{
	delete engine;
	delete messenger;
}

void Gameplay::update()
{
	engine->update();
}
void Gameplay::keyEvent(SDL_Event * e)
{
	messenger->sendMessage( new KeyEvent(e) );
}
void Gameplay::mouseEvent(SDL_Event * e)
{
	messenger->sendMessage( new MouseEvent(e) );
}
void Gameplay::reshape(int width, int height)
{
	render_Reference->reshape(width,height);
}

