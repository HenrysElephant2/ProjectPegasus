#include "Components/Renderable.h"

void Renderable::readFromXML(XmlElement * element) {
	char * programName = NULL;
	std::cout << "reading renderable" << std::endl;
	element->queryStringAttribute("program",(const char**)&programName);
	//std::cout << "Renderable with programName: " << programName << std::endl;
	if(programName != NULL) {
		if(strcmp(programName, "Skybox") == 0) {
			std::cout << "      - Using Skybox program" << std::endl;
			program = ShaderManager::skybox;
		}
	}
}