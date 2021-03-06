#pragma once
//========================================================================
// File: MovementController.h
//
// Part of the Alpha Application
//
// Alpha is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media. 
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// Justin Hunt
//=======================================================================
#include "../Alpha/Interfaces.h"
#include "Geometry.h"

// Forward declarations
class SceneNode;

////////////////////////////////////////////////////
//
// MovementController Description
//
//    Implements a WASD style movement controller
//
//  class MovementController		- Chapter 10, page 281
//
////////////////////////////////////////////////////
class MovementController : public IPointerHandler, public IKeyboardHandler
{
protected:
	Mat4x4  m_matFromWorld;
	Mat4x4	m_matToWorld;
    Mat4x4  m_matPosition;

	Point	m_lastMousePos;
	bool	m_bKey[256]; // Which keys are up and down

	// Orientation Controls
	float		m_fTargetYaw;
	float		m_fTargetPitch;
	float		m_fYaw;
	float		m_fPitch;
	float		m_fPitchOnDown;
	float		m_fYawOnDown;
	float		m_maxSpeed;
	float		m_currentSpeed;

	// Added for Ch19/20 refactor
	bool		m_mouseLButtonDown;
	bool		m_bRotateWhenLButtonDown;

	shared_ptr<SceneNode> m_object;

public:
	MovementController(shared_ptr<SceneNode> object, float initialYaw, float initialPitch, bool rotateWhenLButtonDown);
	void SetObject(shared_ptr<SceneNode> newObject);

	void OnUpdate(DWORD const elapsedMs);

public:
	bool VOnPointerMove(const Point &mousePos, const int radius);
	bool VOnPointerButtonDown(const Point &mousePos, const int radius, const std::string &buttonName);
	bool VOnPointerButtonUp(const Point &mousePos, const int radius, const std::string &buttonName);

	bool VOnKeyDown(const BYTE c) { m_bKey[c] = true; return true; }
	bool VOnKeyUp(const BYTE c) { m_bKey[c] = false; return true; }

	const Mat4x4 *GetToWorld() { return &m_matToWorld; }
	const Mat4x4 *GetFromWorld() { return &m_matFromWorld; }	
};