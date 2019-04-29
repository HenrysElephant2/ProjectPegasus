#include "LevelLoader.h"

LevelLoader::LevelLoader(){

}

void LevelLoader::openLevel(std::string & directory, ECSEngine * engine)
{
	Scene sc = Scene();
	if(directory.compare("") == 0)
		return testLevel(engine);
	std::ifstream file("Levels/" + directory + "/level.pegasus");

	std::cout << "Opening Level: " << directory << std::endl;
	std::string line;
	while(std::getline(file,line))
	{
		std::string filename = "Levels/" + directory + "/" + line;
		sc.openFile(filename);
	}

	int playerID = engine->addEntity();
	std::cout << "Player Entity ID: " << playerID << std::endl;
	Player p = Player(playerID);
	p.cameraOffset = 5;
	glm::vec4 playerPos = glm::vec4(0.0,10.0,0.0,1.0);
	glm::vec3 playerOrientation = glm::vec3(0.0,0.0,0.0);
	Transform t = Transform(playerPos, playerOrientation, 1.0, playerID);
	engine->addTransform(playerID, t);
	engine->addPlayer(playerID,p);


	sc.populate(engine);
}

void LevelLoader::testLevel(ECSEngine * engine)
{
	Scene sc = Scene();
	std::string filename = "Warehouse_Full.fbx";//"AnimationTest2.fbx";//"ShadowTest.fbx";//"testwarehouse.fbx";
	sc.openFile(filename);
	filename = "nugget.fbx";
	//sc.openFile(filename);
	int playerID = engine->addEntity();
	std::cout << "Player Entity ID: " << playerID << std::endl;
	Player p = Player(playerID);
	p.cameraOffset = 5;
	glm::vec4 playerPos = glm::vec4(0.0,10.0,0.0,1.0);
	glm::vec3 playerOrientation = glm::vec3(0.0,0.0,0.0);
	Transform t = Transform(playerPos, playerOrientation, 1.0, playerID);
	engine->addTransform(playerID, t);
	engine->addPlayer(playerID,p);


	sc.populate(engine);
}



void LevelLoader::loadShaders()
{

}