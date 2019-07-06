#ifndef XML_ADAPTER_FACTORY
#define XML_ADAPTER_FACTORY

#include <iostream>
#include "XMLParserAdapter.h"
#include "TinyXMLAdapter.h"

class XmlAdapterFactory {
public:
	static XmlAdapter * getXmlAdapter();
};

#endif