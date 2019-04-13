#include "Gameplay.h"

Gameplay::Gameplay(int width, int height,std::string &level_directory, ShaderManager * shaders_in):State(shaders_in)
{
	std::cout << "Creating ECS" << std::endl;
	engine = new ECSEngine(shaders);
	std::cout << "Created ECS" << std::endl;
	render_Reference = engine->getRenderSystem();
	std::cout << "reshaping window" << std::endl;
	render_Reference->reshape(width,height);
	glClearColor( 0.f, 0.f, 0.f, 1.f );
	std::cout << "Starting to load level" << std::endl;
	LevelLoader level = LevelLoader();
	level.openLevel(level_directory,engine);
	std::cout << "Successfully created ECS" << std::endl;
}

Gameplay::~Gameplay()
{
	delete engine;
}

void Gameplay::update()
{
	engine->update();
}
void Gameplay::keyEvent(SDL_Event * e)
{

}
void Gameplay::mouseEvent(SDL_Event * e)
{

}
void Gameplay::reshape(int width, int height)
{
	render_Reference->reshape(width,height);
}

