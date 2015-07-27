#pragma once
//========================================================================
// XmlResource.h : API to use load Xml files from the Resource Cache
//
// Part of the Alpha Application
//
// Alpha is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media. 
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// Justin Hunt
//========================================================================
#include "ResCache.h"
#include <tinyxml.h>

class XmlResourceExtraData : public IResourceExtraData
{
    TiXmlDocument m_xmlDocument;

public:
    virtual std::string VToString() { return "XmlResourceExtraData"; }
    void ParseXml(char* pRawBuffer);
    TiXmlElement* GetRoot(void) { return m_xmlDocument.RootElement(); }
};

class XmlResourceLoader : public IResourceLoader
{
public:
    virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize) { return rawSize; }
    virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
    virtual std::string VGetPattern() { return "*.xml"; }

    // convenience function
    static TiXmlElement* LoadAndReturnRootXmlElement(const char* resourceString);
};