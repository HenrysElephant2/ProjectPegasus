#include "TinyXMLAdapter.h"


TinyXmlAdapter::TinyXmlAdapter() {
	//document = tinyxml2::XMLDocument();
}

bool TinyXmlAdapter::loadFile(const char* filename) {
	return document.LoadFile(filename);
}

XmlElement* TinyXmlAdapter::getRoot() {
	return (XmlElement*)new TinyXmlElement(document.RootElement());
}



TinyXmlElement::TinyXmlElement(tinyxml2::XMLElement * element):XmlElement() {
	this->element = element;
}

void TinyXmlElement::queryIntAttribute(const char* name, int * i) {
	element->QueryIntAttribute(name,i);
}
void TinyXmlElement::queryDoubleAttribute(const char* name, double * d) {
	element->QueryDoubleAttribute(name,d);
}
void TinyXmlElement::queryFloatAttribute(const char* name, float * f) {
	element->QueryFloatAttribute(name,f);
}
void TinyXmlElement::queryStringAttribute(const char* name, const char ** string) {
	element->QueryStringAttribute(name,string);
}

XmlElement* TinyXmlElement::firstChild(const char* name) {
	tinyxml2::XMLElement * child = element->FirstChildElement(name);
	if(child == NULL) 
		return NULL;
	return (XmlElement*)new TinyXmlElement(child);
}
XmlElement* TinyXmlElement::nextSiblingElement(const char* name) {
	tinyxml2::XMLElement * next = element->NextSiblingElement(name);
	if(next == NULL) 
		return NULL;
	return (XmlElement*)new TinyXmlElement(next);
}

