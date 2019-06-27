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
	std::string filename = "Levels/TestLevel/Warehouse_Full.fbx";//"AnimationTest2.fbx";//"ShadowTest.fbx";//"testwarehouse.fbx";
	sc.openFile(filename);
	filename = "Levels/TestLevel/Nugget.dae";
	sc.openFile(filename);
	int playerID = engine->addEntity();
	std::cout << "Player Entity ID: " << playerID << std::endl;
	Player p = Player(playerID);
	p.cameraOffset = 5;
	glm::vec4 playerPos = glm::vec4(0.0,10.0,0.0,1.0);
	glm::vec3 playerOrientation = glm::vec3(0.0,0.0,0.0);
	Transform t = Transform(playerPos, playerOrientation, 1.0, playerID);
	engine->addTransform(playerID, t);
	engine->addPlayer(playerID,p);

	float *particles = new float[64*12];
	float *tempP = particles;
	for( int i=0; i<64; i++ ) {
		// Location
		*tempP++ = 8;
		float ystart = 6;
		*tempP++ = ystart;
		*tempP++ = 10.3;

		// RGB
		*tempP++ = 1.0;
		*tempP++ = 0.8;
		*tempP++ = 0.2;
		*tempP++ = 1.0;

		// Velocity
		*tempP++ = rand()*4.0/RAND_MAX-2.0;
		float yvel = rand()*4.0/RAND_MAX-2.0;
		*tempP++ = yvel;
		*tempP++ = rand()*4.0/RAND_MAX-2.0;

		// Interaction times
		float y0 = (-yvel - sqrt(yvel*yvel + 4*7.0*ystart)) / (-2*7.0);
		*tempP++ = y0;
		*tempP++ = rand()*1.5/RAND_MAX+0.5;
	}
	GLuint particlesVBO;
	glGenBuffers(1, &particlesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, particlesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (tempP-particles), particles, GL_STATIC_DRAW);

	int psID = engine->addEntity();
	ParticleSystem ps = ParticleSystem( particlesVBO, (tempP-particles)/12, ShaderManager::displayParticles );
	engine->addParticleSystem(psID, ps);

	sc.populate(engine);
}



void LevelLoader::loadShaders()
{

}