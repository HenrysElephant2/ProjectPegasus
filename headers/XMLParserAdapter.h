#ifndef XML_ADAPTER
#define XML_ADAPTER

//forward Declaration
class XmlElement;
class XmlAdapter;

#include <iostream>


class XmlElement {
public:
	virtual void queryIntAttribute(const char* name, int * i) = 0;
	virtual void queryDoubleAttribute(const char* name, double * d) = 0;
	virtual void queryFloatAttribute(const char* name, float * f) = 0;
	virtual void queryStringAttribute(const char* name, const char ** string) = 0;
	virtual XmlElement * firstChild(const char* name) = 0;
	virtual XmlElement * nextSiblingElement(const char* name) = 0;

	virtual ~XmlElement() {}
};


class XmlAdapter {
public:
	virtual bool loadFile(const char*  filename) = 0;
	virtual XmlElement * getRoot() = 0;

	virtual ~XmlAdapter() {}
};


#endif