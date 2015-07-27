#pragma once
//========================================================================
// TransformComponent.h - Component for managing transforms on actors
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
#include "ActorComponent.h"

//---------------------------------------------------------------------------------------------------------------------
// This component implementation is a very simple representation of the physical aspect of an actor.  It just defines 
// the transform and doesn't register with the physics system at all.
//---------------------------------------------------------------------------------------------------------------------
class TransformComponent : public ActorComponent
{
    Mat4x4 m_transform;
public:
	static const char* g_Name;
	virtual const char* VGetName() const { return g_Name; }

    TransformComponent(void) : m_transform(Mat4x4::g_Identity) { }
    virtual bool VInit(TiXmlElement* pData) override;
    virtual TiXmlElement* VGenerateXml(void) override;

    // transform functions
    Mat4x4 GetTransform(void) const { return m_transform; }
    void SetTransform(const Mat4x4& newTransform) { m_transform = newTransform; }
    Vec3 GetPosition(void) const { return m_transform.GetPosition(); }
    void SetPosition(const Vec3& pos) { m_transform.SetPosition(pos); }

	//inline void BuildYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians)

	//Vec3 GetYawPitchRoll(void) const { }
    void SetYawPitchRoll(const Vec3& ypr) { m_transform.BuildYawPitchRoll(ypr.x, ypr.y, ypr.z); }
    Vec3 GetLookAt(void) const { return m_transform.GetDirection(); }
};
