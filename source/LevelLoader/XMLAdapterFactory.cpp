#include "XMLAdapterFactory.h"

XmlAdapter * XmlAdapterFactory::getXmlAdapter() {
	return (XmlAdapter *) new TinyXmlAdapter();
}