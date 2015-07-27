//========================================================================
// XmlResource.cpp : API to use load Xml files from the Resource Cache
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
#include "AlphaStd.h"
#include "XmlResource.h"

void XmlResourceExtraData::ParseXml(char* pRawBuffer)
{
    m_xmlDocument.Parse(pRawBuffer);
}

bool XmlResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
    if (rawSize <= 0)
        return false;

    shared_ptr<XmlResourceExtraData> pExtraData = shared_ptr<XmlResourceExtraData>(AC_NEW XmlResourceExtraData());
    pExtraData->ParseXml(rawBuffer);

    handle->SetExtra(shared_ptr<XmlResourceExtraData>(pExtraData));

    return true;
}

shared_ptr<IResourceLoader> CreateXmlResourceLoader()
{
    return shared_ptr<IResourceLoader>(AC_NEW XmlResourceLoader());
}

TiXmlElement* XmlResourceLoader::LoadAndReturnRootXmlElement(const char* resourceString)
{
    Resource resource(resourceString);
    shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);  // this actually loads the XML file from the zip file
    shared_ptr<XmlResourceExtraData> pExtraData = static_pointer_cast<XmlResourceExtraData>(pResourceHandle->GetExtra());
    return pExtraData->GetRoot();
}