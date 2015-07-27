#pragma once
//========================================================================
// PhysicsComponent.h - Component for physical movement and collision detection
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

class TiXmlElement;

class PhysicsComponent : public ActorComponent
{
public:
	const static char *g_Name;
	virtual const char *VGetName() const override { return PhysicsComponent::g_Name; }

public:
    PhysicsComponent(void);
    virtual ~PhysicsComponent(void);
    virtual TiXmlElement* VGenerateXml(void) override;

    // ActorComponent interface
    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit(void) override;
    virtual void VUpdate(int deltaMs) override;

    // Physics functions
    void ApplyForce(const Vec3& direction, float forceNewtons);
    void ApplyTorque(const Vec3& direction, float forceNewtons);
	bool KinematicMove(const Mat4x4& transform);

    // acceleration
    void ApplyAcceleration(float acceleration);
    void RemoveAcceleration(void);
    void ApplyAngularAcceleration(float acceleration);
    void RemoveAngularAcceleration(void);

	//void RotateY(float angleRadians);
    Vec3 GetVelocity(void);
    void SetVelocity(const Vec3& velocity);
    void RotateY(float angleRadians);
    void SetPosition(float x, float y, float z);
    void Stop(void);


protected:
    void CreateShape();
    void BuildRigidBodyTransform(TiXmlElement* pTransformElement);

    float m_acceleration, m_angularAcceleration;
    float m_maxVelocity, m_maxAngularVelocity;

	std::string m_shape;
    std::string m_density;
    std::string m_material;
	
	Vec3 m_RigidBodyLocation;		// this isn't world position! This is how the rigid body is offset from the position of the actor.
	Vec3 m_RigidBodyOrientation;	// ditto, orientation
	Vec3 m_RigidBodyScale;			// ditto, scale
	
    shared_ptr<IGamePhysics> m_pGamePhysics;  // might be better as a weak ptr...
};