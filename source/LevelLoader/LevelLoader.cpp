#include "LevelLoader.h"

LevelLoader::LevelLoader(){

}

void LevelLoader::openLevel(std::string & directory, ECSEngine * engine)
{
	return testLevel(engine);
}

void LevelLoader::testLevel(ECSEngine * engine)
{
	Scene sc = Scene();
	std::string filename = "testwarehouse.fbx";
	sc.openFile(filename);
	int playerID = engine->addEntity();
	std::cout << "Player Entity ID: " << playerID << std::endl;
	Player p = Player(playerID);
	p.cameraOffset = 25;
	glm::vec4 playerPos = glm::vec4(0.0,0.0,0.0,1.0);
	glm::vec3 playerOrientation = glm::vec3(0.0,0.0,0.0);
	Transform t = Transform(playerPos, playerOrientation, 1.0, playerID);
	engine->addTransform(playerID, t);
	engine->addPlayer(playerID,p);
	addUnused(engine, &sc);
}

void LevelLoader::addUnused(ECSEngine * engine, Scene * sc)
{
	int unusedCount = sc->getUnusedCount();
	for(int i = 0; i < unusedCount; i++)
	{
		int entity = engine->addEntity();
		ComponentWrapper * wrapper = sc->getUnused(entity);
		engine->addTransform(entity, wrapper->t);
		engine->addRenderable(entity, wrapper->r);

	}
}

// determine the correct shader to use for material m. decision is based on the information that the material has.
// eg if it has a normal map, it should use the normal mapping shader. This function should only be called if the level file does not provide a specific shader
int LevelLoader::determineShader(Material &m)
{
	return 0;
}

void LevelLoader::loadShaders()
{

}