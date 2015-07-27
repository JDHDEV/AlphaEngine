#pragma once
//========================================================================
// ScriptResource.h
//
// Part of the Alpha Application
//
// Alpha is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media. 
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// Justin Hunt
//
//========================================================================

// --------------------------------------------------------------------------
// File:        ScriptResource.h
//
// Purpose:     The declaration of a quick'n dirty ZIP file reader class.
//              Original code from Javier Arevalo.
//              Get zlib from http://www.cdrom.com/pub/infozip/zlib/
//
// class XmlFile - Chapter 7, page 186
// --------------------------------------------------------------------------
#include "../ResourceCache/ResCache.h"

class ScriptResourceLoader : public IResourceLoader
{
public:
    virtual bool VUseRawFile() { return false; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual bool VAddNullZero() { return true; }
    virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize) { return rawSize; }
    virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
    virtual std::string VGetPattern() { return "*.lua"; }
};