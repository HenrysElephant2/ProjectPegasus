#include "Components/Renderable.h"

void Renderable::readFromXML(XmlElement * element) {
	char * programName = NULL;
	element->queryStringAttribute("program",(const char**)&programName);
	if(programName != NULL) {
		if(strcmp(programName, "Skybox") == 0) {
			program = ShaderManager::skybox;
		}
	}
}