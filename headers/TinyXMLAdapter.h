#ifndef TINY_XML_ADAPTER
#define TINY_XML_ADAPTER



#include "XMLParserAdapter.h"
#include <tinyxml2.h>
#include <iostream>




class TinyXmlAdapter: XmlAdapter {
private:
	tinyxml2::XMLDocument document;

public:
	TinyXmlAdapter();
	bool loadFile(const char* filename);
	XmlElement * getRoot();
};


class TinyXmlElement: public XmlElement {
private:
	tinyxml2::XMLElement * element;

public:
	TinyXmlElement(tinyxml2::XMLElement * element);

	void queryIntAttribute(const char* name, int * i);
	void queryDoubleAttribute(const char* name, double * d);
	void queryFloatAttribute(const char* name, float * f);
	void queryStringAttribute(const char* name, const char ** string);

	XmlElement * firstChild(const char* name);
	XmlElement * nextSiblingElement(const char* name);
};

#endif