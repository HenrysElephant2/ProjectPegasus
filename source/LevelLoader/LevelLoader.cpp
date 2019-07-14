#include "LevelLoader.h"

LevelLoader::LevelLoader(){

}

void LevelLoader::openLevel(std::string & directory, ECSEngine * engine)
{
	// Scene sc = Scene();
	if(directory.compare("") == 0)
		directory = "TestLevel";

	XmlAdapter * parser = XmlAdapterFactory::getXmlAdapter();
	std::string xmlFilename = "Levels/" + directory + "/level.xml";
	if(parser->loadFile(xmlFilename.c_str()) == 0) {
		LevelFileVerification verification = LevelFileVerification();
		std::cout << "Successfully opened file: Levels/" << directory << "/level.xml" << std::endl;
		XmlElement * root = parser->getRoot();
		XmlElement * sceneElement = root->firstChild("scene");
		
		while(sceneElement != NULL) {
			char * filename = NULL;
			sceneElement->queryStringAttribute("file", (const char **)&filename);

			if(filename != NULL) {
				std::string filenameAsString("Levels/" + directory + "/" + filename);
				Scene sc = Scene();
				if(sc.openFile(filenameAsString)) {

					verification.hasValidScene = true;

					XmlElement * entity = sceneElement->firstChild("entity");
					while(entity != NULL) {
						loadEntity(entity,&sc,engine,verification);


						XmlElement * tempEntity = entity;
						entity = entity->nextSiblingElement("entity");
						//delete entity;
					}
					sc.populate(engine);
				}
			}
			else std::cout << "Error: Scene XML Element did not have a filename attribute" << std::endl;

			XmlElement * tempScene = sceneElement;
			sceneElement = sceneElement->nextSiblingElement("scene");
			//delete tempScene;
		}

		if(!verification.isValid()) {
			std::cout << "Level file is invalid and could not be opened" << std::endl;
			exit(1);
		}

	}
	else {
		std::cout << "Could not open level file: " << xmlFilename << std::endl;
		exit(1);
	}

}

void LevelLoader::loadEntity(XmlElement * entity, Scene * scene, ECSEngine * engine, LevelFileVerification & verification) {

	EntityManager * entityManager = EntityManager::getEntityManager();

	std::cout << "Initializing Entity" << std::endl;
	int entityID;
	XmlElement * renderableElement = NULL;

	bool addedFromFile = false;

	renderableElement = entity->firstChild("renderable");
	if(renderableElement != NULL) {
		std::cout << "   - Adding Renderable" << std::endl;
		char * meshName = NULL;
		renderableElement->queryStringAttribute("meshName",(const char **) &meshName);
		
		std::string meshAsString(meshName);
		entityID = scene->populateByName(meshAsString,engine);
		addedFromFile = true;
		Renderable dummy = Renderable();
		Renderable * renderable = entityManager->getComponentManager(dummy)->getComponent(entityID);//engine->getRenderableManager()->getComponent(entityID);
		if(renderable != NULL) {
			renderable->readFromXML(renderableElement);
		}

		
		delete renderableElement;
	}
	else entityID = engine->addEntity();
	std::cout << "   - entityID: " << entityID << std::endl;

	XmlElement * playerElement = entity->firstChild("player");
	if(playerElement != NULL) {
		std::cout << "   - Adding Player" << std::endl;

		Player player = Player(entityID);

		player.readFromXML(playerElement);
		entityManager->addComponent(entityID,player);
		verification.hasPlayer = true;
		delete playerElement;
	}

	XmlElement * transformElement = entity->firstChild("transform");
	if(transformElement != NULL) {
		std::cout << "   - Adding Transform" << std::endl;
		Transform dummy = Transform();
		Transform * t = entityManager->getComponentManager(dummy)->getComponent(entityID);
		if(t == NULL) {
			Transform temp = Transform();
			entityManager->addComponent(entityID,temp);
			t = entityManager->getComponentManager(dummy)->getComponent(entityID);
		}

		t->readFromXML(transformElement);
		delete transformElement;
	}

	XmlElement * lightElement = entity->firstChild("light");
	if(lightElement != NULL) {
		std::cout << "   - Adding Light" << std::endl;

		Light dummy = Light();
		Light * light = entityManager->getComponentManager(dummy)->getComponent(entityID);//engine->getLightManager()->getComponent(entityID);

		if(light == NULL) {
			Light temp = Light();
			entityManager->addComponent(entityID,temp);
			light = entityManager->getComponentManager(dummy)->getComponent(entityID);;
		}

		light->readFromXML(lightElement);
		delete lightElement;
	}
}

bool LevelLoader::LevelFileVerification::isValid() {
	return hasValidScene && hasPlayer;
}

// void LevelLoader::testLevel(ECSEngine * engine)
// {
// 	std::string levelname = "TestLevel";
// 	openLevel(levelname,engine);
// 	std::cout << "Finished Loading Test Level" << std::endl;
	// Scene sc = Scene();
	// std::string filename = "Levels/TestLevel/Warehouse_Full.fbx";//"AnimationTest2.fbx";//"ShadowTest.fbx";//"testwarehouse.fbx";
	// sc.openFile(filename);
	// filename = "Levels/TestLevel/Nugget3.dae";
	// sc.openFile(filename);
	// filename = "Levels/TestLevel/NuggetAnimated2.fbx";
	// //sc.openFile(filename);
	// filename = "Levels/TestLevel/falcon.fbx";
	// sc.openFile(filename);
	// int playerID = engine->addEntity();
	// std::cout << "Player Entity ID: " << playerID << std::endl;
	// Player p = Player(playerID);
	// p.cameraOffset = 5;
	// glm::vec4 playerPos = glm::vec4(0.0,10.0,0.0,1.0);
	// glm::vec3 playerOrientation = glm::vec3(0.0,0.0,0.0);
	// Transform t = Transform(playerPos, playerOrientation, 1.0, playerID);
	// engine->addTransform(playerID, t);
	// engine->addPlayer(playerID,p);

	// float *particles = new float[64*12];
	// float *tempP = particles;
	// for( int i=0; i<64; i++ ) {
	// 	// Location
	// 	*tempP++ = 8;
	// 	float ystart = 6;
	// 	*tempP++ = ystart;
	// 	*tempP++ = 10.3;

	// 	// RGB
	// 	*tempP++ = 1.0;
	// 	*tempP++ = 0.8;
	// 	*tempP++ = 0.2;
	// 	*tempP++ = 1.0;

	// 	// Velocity
	// 	*tempP++ = rand()*4.0/RAND_MAX-2.0;
	// 	float yvel = rand()*4.0/RAND_MAX-2.0;
	// 	*tempP++ = yvel;
	// 	*tempP++ = rand()*4.0/RAND_MAX-2.0;

	// 	// Interaction times
	// 	float y0 = (-yvel - sqrt(yvel*yvel + 4*7.0*ystart)) / (-2*7.0);
	// 	*tempP++ = y0;
	// 	*tempP++ = rand()*1.5/RAND_MAX+0.5;
	// 	std::cout << yvel << " " << y0 << std::endl;
	// }
	// std::cout << (tempP-particles)/12 << std::endl;
	// GLuint particlesVBO;
	// glGenBuffers(1, &particlesVBO);
	// glBindBuffer(GL_ARRAY_BUFFER, particlesVBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (tempP-particles), particles, GL_STATIC_DRAW);

	// int psID = engine->addEntity();
	// ParticleSystem ps = ParticleSystem( particlesVBO, (tempP-particles)/12, ShaderManager::displayParticles );
	// engine->addParticleSystem(psID, ps);

	// sc.populate(engine);
// }

