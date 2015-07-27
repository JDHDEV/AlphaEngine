//========================================================================
// SceneNodes.cpp - defines the base class for the 3D graphics scene graph, and
//                  derived classes like RootNode, Alpha scene nodes, Camera, Grids, etc.                  
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

#include "../Alpha/Alpha.h"
#include "../Actors/ActorComponent.h"
#include "../Actors/RenderComponent.h"
#include "../Actors/TransformComponent.h"

#include "D3DRenderer.h"
#include "Geometry.h"
#include "Lights.h"
//#include "Mesh.h"
//#include "Raycast.h"
#include "OctTree.h"
#include "Shaders.h"
#include "../DirectX11Wrapper/DirectX11Renderable.h"
#include "tchar.h"
#include "../ResourceCache/ResCache.h"

#include "../DirectX11Wrapper/DirectX11Vertex.h"
#include "../DirectX11Wrapper/DirectX11Pixel.h"

#include <xnamath.h>

// move to scene node.cpp
////////////////////////////////////////////////////
// BoundingBox Implementation
////////////////////////////////////////////////////
BoundingBox::BoundingBox()
{
	m_Center = Vec3();
    m_Radius = 1.0f;
}

BoundingBox::BoundingBox(Vec3 min, Vec3 max)
{
	m_Center = max + min;
	m_Center = m_Center/2;
	Vec3 temp = max - m_Center;
	m_Radius = std::max(std::max(temp.x, temp.y),temp.z);
}

void BoundingBox::Set(Vec3 min, Vec3 max)
{
	m_Center = max + min;
	m_Center = m_Center/2;
	Vec3 temp = max - m_Center;
	m_Radius = std::max(std::max(temp.x, temp.y),temp.z);
}

ContainmentType BoundingBox::Contains(BoundingBox* pBoundingBox)
{
	float DistanceX = abs(pBoundingBox->m_Center.x-m_Center.x);
	float DistanceY = abs(pBoundingBox->m_Center.y-m_Center.y);
	float DistanceZ = abs(pBoundingBox->m_Center.z-m_Center.z);
	float Distance = m_Radius+pBoundingBox->m_Radius;
	float Diff = m_Radius-pBoundingBox->m_Radius;

	if((DistanceX >= Distance)||(DistanceY >= Distance)||(DistanceZ >= Distance))
		return Alpha_Disjoint;
	//if((DistanceX < Distance)&&(DistanceY < Distance)&&(DistanceZ < Distance))
	//{
		if((DistanceX <= Diff)&&(DistanceY <= Diff)&&(DistanceZ <= Diff))
		{
			return Alpha_Contains;
		}
		return Alpha_Intersects;
	//}	
}

ContainmentType BoundingBox::Contains(BoundingBox boundingBox)
{
	float DistanceX = abs(boundingBox.m_Center.x-m_Center.x);
	float DistanceY = abs(boundingBox.m_Center.y-m_Center.y);
	float DistanceZ = abs(boundingBox.m_Center.z-m_Center.z);
	float Distance = m_Radius+boundingBox.m_Radius;
	float Diff = m_Radius-boundingBox.m_Radius;

	if((DistanceX >= Distance)||(DistanceY >= Distance)||(DistanceZ >= Distance))
		return Alpha_Disjoint;
	//if((DistanceX < Distance)&&(DistanceY < Distance)&&(DistanceZ < Distance))
	//{
		if((DistanceX <= Diff)&&(DistanceY <= Diff)&&(DistanceZ <= Diff))
		{
			return Alpha_Contains;
		}
		return Alpha_Intersects;
	//}	
}

ContainmentType BoundingBox::Contains(BoundingSphere* pBoundingSphere)
{
	Vec3 Distance = m_Center - pBoundingSphere->m_Center;
	float DistanceSquared = Distance.Dot(Distance);

	if(DistanceSquared >= ((m_Radius+pBoundingSphere->m_Radius)*(m_Radius+pBoundingSphere->m_Radius)))
		return Alpha_Disjoint;

	if(DistanceSquared <= ((m_Radius-pBoundingSphere->m_Radius)*(m_Radius-pBoundingSphere->m_Radius)))
		return Alpha_Contains;

	return Alpha_Intersects;
}

ContainmentType BoundingBox::Contains(BoundingSphere boundingSphere)
{
	Vec3 Distance = m_Center - boundingSphere.m_Center;
	float DistanceSquared = Distance.Dot(Distance);

	if(DistanceSquared >= ((m_Radius+boundingSphere.m_Radius)*(m_Radius+boundingSphere.m_Radius)))
		return Alpha_Disjoint;

	if(DistanceSquared <= ((m_Radius-boundingSphere.m_Radius)*(m_Radius-boundingSphere.m_Radius)))
		return Alpha_Contains;

	return Alpha_Intersects;
}

Vec3 BoundingBox::MaxVec3()
{
	return Vec3(m_Center.x+m_Radius, m_Center.y+m_Radius, m_Center.z+m_Radius);
}

Vec3 BoundingBox::MinVec3()
{
	return Vec3(m_Center.x-m_Radius, m_Center.y-m_Radius, m_Center.z-m_Radius);
}

Vec3 BoundingBox::Dimensions()
{
	return Vec3(m_Radius*2, m_Radius*2, m_Radius*2);
}

////////////////////////////////////////////////////
// BoundingSphere Implementation
////////////////////////////////////////////////////
BoundingSphere::BoundingSphere()
{
	m_Center = Vec3();
    m_Radius = 1.42f; // distance from corner to center for default bounding box
}

BoundingSphere::BoundingSphere(Vec3 min, Vec3 max) //TODO
{
	m_Center = max + min;
	m_Center = m_Center/2;

	Vec3 Distance = m_Center - min;
	float DistanceSquared = Distance.Dot(Distance);
	m_Radius = std::sqrt(DistanceSquared);
}

////////////////////////////////////////////////////
// OctTreeNodeProperties Implementation
////////////////////////////////////////////////////
OctTreeNodeProperties::OctTreeNodeProperties(void)
{
	m_BoundingSphere.m_Radius = 0;
    m_RenderPass = RenderPass_0;
}

//
// OctTreeNodeProperties::Transform			- Chapter 16, page 528
//
void OctTreeNodeProperties::Transform(Mat4x4 *toWorld, Mat4x4 *fromWorld) const
{
	if (toWorld)
		*toWorld = m_ToWorld;
	
	if (fromWorld)
		*fromWorld = m_FromWorld;
}

////////////////////////////////////////////////////
// OctTreeNode Implementation
////////////////////////////////////////////////////
bool OctTreeNode::m_TreeReady = false; 
bool OctTreeNode::m_TreeBuilt = false; 
PhysicalObjectQueue OctTreeNode::m_PendingInsertion;// = new std::queue<shared_ptr<ISceneNode>>();

OctTreeNode::OctTreeNode(BoundingBox region, LeafObjectList objList) : //, RenderPass renderPass,  const Mat4x4 *to, const Mat4x4 *from) :
	MIN_SIZE(1)
{
	m_Props.m_BoundingBox = region;
	m_Objects = objList;
	m_ActiveNodes = 0;
	m_CurLife = -1;

	//m_Props.m_RenderPass = renderPass;
	//VSetTransform(to, from); 
}

OctTreeNode::OctTreeNode(BoundingBox region) : //, RenderPass renderPass,  const Mat4x4 *to, const Mat4x4 *from) :
	MIN_SIZE(1)
{
	m_Props.m_BoundingBox = region;
	m_ActiveNodes = 0;
	m_CurLife = -1;

	//m_Props.m_RenderPass = renderPass;
	//VSetTransform(to, from); 
}

OctTreeNode::OctTreeNode() : //RenderPass renderPass,  const Mat4x4 *to, const Mat4x4 *from) :
	MIN_SIZE(1)
{
	m_ActiveNodes = 0;
	m_MaxLifespan = 8;
	m_CurLife = -1;

	//m_Props.m_RenderPass = renderPass;
	//VSetTransform(to, from); 
}

OctTreeNode::~OctTreeNode()
{

}

OctTreeNodePtr OctTreeNode::CreateNode(BoundingBox region, LeafObjectList objList)
{
	if (objList.size() == 0)
		return nullptr;
	OctTreeNodePtr ret = OctTreeNodePtr(new OctTreeNode(region, objList));
	ret->m_pParent = (OctTreeNodePtr)this;
	return ret;
}

OctTreeNodePtr OctTreeNode::CreateNode(BoundingBox region, PhysicalObjectPtr Item)
{
	LeafObjectList objList;
	objList.push_front(Item);
	OctTreeNodePtr ret = OctTreeNodePtr(new OctTreeNode(region, objList));
	ret->m_pParent = (OctTreeNodePtr)this;
	return ret;
}

//
// OctTreeNode::VOnRestore
//	This is meant to be called from any class that inherits from SceneNode and overloads VOnRestore()
//
HRESULT OctTreeNode::VOnRestore(Scene *pScene)
{
	if (m_TreeBuilt == true)
	{
		LeafObjectList::iterator i = m_Objects.begin();
		LeafObjectList::iterator end = m_Objects.end();
		while (i != end)
		{
			(*i)->VOnRestore(pScene);
			++i;
		}

		for (int a = 0; a < 8; a++)
		{
				//m_ActiveNodes |= (byte)(1 << a);
			if(m_ChildNodes[a] != NULL)
				m_ChildNodes[a]->VOnRestore(pScene);

		}
	}
	return S_OK;
}

//
// OctTreeNode::VOnLostDevice
//
HRESULT OctTreeNode::VOnLostDevice(Scene *pScene)
{
	LeafObjectList::iterator i = m_Objects.begin();
	LeafObjectList::iterator end = m_Objects.end();
	while (i != end)
	{
		(*i)->VOnLostDevice(pScene);
		++i;
	}

	for (int a = 0; a < 8; a++)
	{
			//m_ActiveNodes |= (byte)(1 << a);
			m_ChildNodes[a]->VOnLostDevice(pScene);
	}
	return S_OK;
}

//
// OctTreeNode::VSetTransform()
//
void OctTreeNode::VSetTransform(const Mat4x4 *toWorld, const Mat4x4 *fromWorld)
{
	m_Props.m_ToWorld = *toWorld;
	if (!fromWorld)
	{
		m_Props.m_FromWorld = m_Props.m_ToWorld.Inverse();
	}
	else
	{
		m_Props.m_FromWorld = *fromWorld;
	}
}

//
// OctTreeNode::VPreRender
//
HRESULT OctTreeNode::VPreRender(Scene *pScene) 
{
	pScene->PushAndSetMatrix(m_Props.m_ToWorld);
	return S_OK;
}

//
// OctTreeNode::VPostRender	
//
HRESULT OctTreeNode::VPostRender(Scene *pScene) 
{
	pScene->PopMatrix();
	return S_OK;
}

//
// OctTreeNode::VIsVisible	
//
bool OctTreeNode::VIsVisible(Scene *pScene) const
{
	// transform the location of this node into the camera space 
	// of the camera attached to the scene

	Mat4x4 toWorld, fromWorld;
	pScene->GetCamera()->VGet()->Transform(&toWorld, &fromWorld);

	Vec3 pos = GetWorldPosition();

	Vec3 fromWorldPos = fromWorld.Xform(pos);

	Frustum const &frustum = pScene->GetCamera()->GetFrustum();

	bool isVisible = frustum.Inside(fromWorldPos, m_Props.m_BoundingSphere.m_Radius);
	return isVisible;
}

//
// SceneNode::GetWorldPosition			- not described in the book
//
//   This was added post press to respect any SceneNode ancestors - you have to add all 
//   their positions together to get the world position of any SceneNode.
//
const Vec3 OctTreeNode::GetWorldPosition() const
{
	Vec3 pos = GetPosition();
	if (m_pParent)
	{
		pos += m_pParent->GetWorldPosition();
	}
	return pos;
}

//
// OctTreeNode::VOnUpdate
//
HRESULT OctTreeNode::VOnUpdate(Scene *pScene, DWORD const elapsedMs)
{
	if (m_TreeBuilt == true)
	{
		//Start a count down death timer for any leaf nodes which don't have objects or children.
		//when the timer reaches zero, we delete the leaf. If the node is reused before death, we double its lifespan.
		//this gives us a "frequency" usage score and lets us avoid allocating and deallocating memory unnecessarily
		if (m_Objects.size() == 0)
		{
			if (m_ActiveNodes == 0)
			{
				if (m_CurLife == -1)
					m_CurLife = m_MaxLifespan;
				else if (m_CurLife > 0)
				{
					m_CurLife--;
				}
			}
		}
		else
		{
			if (m_CurLife != -1)
			{
				if(m_MaxLifespan <= 64)
					m_MaxLifespan *= 2;
				m_CurLife = -1;
			}
		}
		//list<Physical> movedObjects = new List<Physical>(m_objects.Count);
		LeafObjectList movedObjects(0);//m_Objects.size());

		//go through and update every object in the current tree node
		for (LeafObjectList::const_iterator obj = m_Objects.begin(), end = m_Objects.end(); obj != end; ++obj)
		{
			//we should figure out if an object actually moved so that we know whether we need to update this node in the tree.
			(*obj)->VOnUpdate(pScene, elapsedMs);

			if ((*obj)->VHasMoved())
			{
				movedObjects.push_front(*obj);
			}
		}

		////prune any dead objects from the tree. i handle deletions different but i may come back to this idk
		//int listSize = m_Objects.size();
		//for (int a = 0; a < listSize; a++)
		//{
		//	if (!m_Objects[a].Alive)
		//	{
		//		if (movedObjects.Contains(m_objects[a]))
		//			movedObjects.Remove(m_objects[a]);
		//		m_objects.RemoveAt(a--);
		//		listSize--;
		//	}
		//}

		//recursively update any child nodes.
		for(int flags = m_ActiveNodes, index = 0; flags > 0; flags >>=1, index++)
		{
			if ((flags & 1) == 1) m_ChildNodes[index]->VOnUpdate(pScene, elapsedMs);
		}

		//If an object moved, we can insert it into the parent and that will insert it into the correct tree node.
		//note that we have to do this last so that we don't accidentally update the same object more than once per frame.
		for (LeafObjectList::const_iterator obj = movedObjects.begin(), end = movedObjects.end(); obj != end; ++obj)
		{
			OctTreeNodePtr current = m_pParent;

			//figure out how far up the tree we need to go to reinsert our moved object
			//we are either using a bounding rect or a bounding sphere
			//try to move the object into an enclosing parent node until we've got full containment
			if ((*obj)->VGetBox() != nullptr)
			{
				if(m_Props.m_BoundingBox.Contains((*obj)->VGetBox()) != Alpha_Contains)
				{
					while (current->m_Props.m_BoundingBox.Contains((*obj)->VGetBox()) != Alpha_Contains)
					{
						if (current->m_pParent != nullptr) 
						{
							current = current->m_pParent;
						}
						else 
						{
							break; //prevent infinite loops when we go out of bounds of the root node region
						}
					}
				}
			}
			else //we must be using a bounding sphere, so check for its containment.
			{
				if(m_Props.m_BoundingBox.Contains((*obj)->VGetSphere()) != Alpha_Contains)
				{
					while (current->m_Props.m_BoundingBox.Contains((*obj)->VGetSphere()) != Alpha_Contains)
					{
						if (current->m_pParent != nullptr) 
						{
							current = current->m_pParent;
						}
						else 
						{
							break; //prevent infinite loops when we go out of bounds of the root node region
						}
					}
				}
			}

			//now, remove the object from the current node and insert it into the current containing node.
			m_Objects.remove(*obj);
			current->VAddChild(*obj);   //this will try to insert the object as deep into the tree as we can go.
		}

		//prune out any dead branches in the tree
		for (int flags = m_ActiveNodes, index = 0; flags > 0; flags >>= 1, index++)
		{
			if ((flags & 1) == 1 && m_ChildNodes[index]->m_CurLife == 0) 
			{
				m_ChildNodes[index] = nullptr;
				m_ActiveNodes ^= (byte)(1 << index);       //remove the node from the active nodes flag list
			}
		}

		////now that all objects have moved and they've been placed into their correct nodes in the octree, we can look for collisions.
		//if (IsRoot == true)
		//{
		//	//This will recursively gather up all collisions and create a list of them.
		//	//this is simply a matter of comparing all objects in the current root node with all objects in all child nodes.
		//	//note: we can assume that every collision will only be between objects which have moved.
		//	//note 2: An explosion can be centered on a point but grow in size over time. In this case, you'll have to override the update method for the explosion.
		//	List<IntersectionRecord> irList = GetIntersection(new List<Physical>());

		//	foreach (IntersectionRecord ir in irList)
		//	{
		//		if (ir.PhysicalObject != null)
		//			ir.PhysicalObject.HandleIntersection(ir);
		//		if (ir.OtherPhysicalObject != null)
		//			ir.OtherPhysicalObject.HandleIntersection(ir);
		//	}
		//}
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
}

//
// OctTreeNode::VAddChild()
//	A tree has already been created, so we're going to try to insert an item into the tree without rebuilding the whole thing
//
bool OctTreeNode::VAddChild(PhysicalObjectPtr node)
{
	/*make sure we're not inserting an object any deeper into the tree than we have to.
		-if the current node is an empty leaf node, just insert and leave it.*/
	if (m_Objects.size() <= 1 && m_ActiveNodes == 0)
	{
		m_Objects.push_front(node);
		return true;
	}

	Vec3 dimensions = m_Props.m_BoundingBox.Dimensions();
	//Check to see if the dimensions of the box are greater than the minimum dimensions
	if (dimensions.x <= MIN_SIZE && dimensions.y <= MIN_SIZE && dimensions.z <= MIN_SIZE)
	{
		m_Objects.push_front(node);
		return true;
	}
	//Vec3 half = dimensions / 2.0f;

	//Find or create subdivided regions for each octant in the current region
	std::array<BoundingBox, 8> childOctant;
	childOctant[0] = (m_ChildNodes[0] != nullptr) ? m_ChildNodes[0]->m_Props.m_BoundingBox : BoundingBox(m_Props.m_BoundingBox.MinVec3(), m_Props.m_BoundingBox.m_Center);
	childOctant[1] = (m_ChildNodes[1] != nullptr) ? m_ChildNodes[1]->m_Props.m_BoundingBox : BoundingBox(Vec3(m_Props.m_BoundingBox.m_Center.x, m_Props.m_BoundingBox.MinVec3().y, m_Props.m_BoundingBox.MinVec3().z), Vec3(m_Props.m_BoundingBox.MaxVec3().x, m_Props.m_BoundingBox.m_Center.y, m_Props.m_BoundingBox.m_Center.z));
	childOctant[2] = (m_ChildNodes[2] != nullptr) ? m_ChildNodes[2]->m_Props.m_BoundingBox : BoundingBox(Vec3(m_Props.m_BoundingBox.m_Center.x, m_Props.m_BoundingBox.MinVec3().y, m_Props.m_BoundingBox.m_Center.z), Vec3(m_Props.m_BoundingBox.MaxVec3().x, m_Props.m_BoundingBox.m_Center.y, m_Props.m_BoundingBox.MaxVec3().z));
	childOctant[3] = (m_ChildNodes[3] != nullptr) ? m_ChildNodes[3]->m_Props.m_BoundingBox : BoundingBox(Vec3(m_Props.m_BoundingBox.MinVec3().x, m_Props.m_BoundingBox.MinVec3().y, m_Props.m_BoundingBox.m_Center.z), Vec3(m_Props.m_BoundingBox.m_Center.x, m_Props.m_BoundingBox.m_Center.y, m_Props.m_BoundingBox.MaxVec3().z));
	childOctant[4] = (m_ChildNodes[4] != nullptr) ? m_ChildNodes[4]->m_Props.m_BoundingBox : BoundingBox(Vec3(m_Props.m_BoundingBox.MinVec3().x, m_Props.m_BoundingBox.m_Center.y, m_Props.m_BoundingBox.MinVec3().z), Vec3(m_Props.m_BoundingBox.m_Center.x, m_Props.m_BoundingBox.MaxVec3().y, m_Props.m_BoundingBox.m_Center.z));
	childOctant[5] = (m_ChildNodes[5] != nullptr) ? m_ChildNodes[5]->m_Props.m_BoundingBox : BoundingBox(Vec3(m_Props.m_BoundingBox.m_Center.x, m_Props.m_BoundingBox.m_Center.y, m_Props.m_BoundingBox.MinVec3().z), Vec3(m_Props.m_BoundingBox.MaxVec3().x, m_Props.m_BoundingBox.MaxVec3().y, m_Props.m_BoundingBox.m_Center.z));
	childOctant[6] = (m_ChildNodes[6] != nullptr) ? m_ChildNodes[6]->m_Props.m_BoundingBox : BoundingBox(m_Props.m_BoundingBox.m_Center, m_Props.m_BoundingBox.MaxVec3());
	childOctant[7] = (m_ChildNodes[7] != nullptr) ? m_ChildNodes[7]->m_Props.m_BoundingBox : BoundingBox(Vec3(m_Props.m_BoundingBox.MinVec3().x, m_Props.m_BoundingBox.m_Center.y, m_Props.m_BoundingBox.m_Center.z), Vec3(m_Props.m_BoundingBox.m_Center.x, m_Props.m_BoundingBox.MaxVec3().y, m_Props.m_BoundingBox.MaxVec3().z));

	//First, is the item completely contained within the root bounding box?
	//note2: I shouldn't actually have to compensate for this. If an object is out of our predefined bounds, then we have a problem/error.
	//          Wrong. Our initial bounding box for the terrain is constricting its height to the highest peak. Flying units will be above that.
	//             Fix: I resized the enclosing box to 256x256x256. This should be sufficient.
	if (node->VGetBox() != nullptr && m_Props.m_BoundingBox.Contains(node->VGetBox()) == Alpha_Contains)
	{
		bool found = false;
		//we will try to place the object into a child node. If we can't fit it in a child node, then we insert it into the current node object list.
		for(int a=0; a<8 ; a++)
		{
			//is the object fully contained within a quadrant?
			if (childOctant[a].Contains(node->VGetBox()) == Alpha_Contains)
			{
				if (m_ChildNodes[a] != nullptr)
					m_ChildNodes[a]->VAddChild(node);   //Add the item into that tree and let the child tree figure out what to do with it
				else
				{
					m_ChildNodes[a] = CreateNode(childOctant[a], node);   //create a new tree node with the item
					m_ActiveNodes |= (byte)(1 << a);
				}
				found = true;
			}
		}
		if(!found) m_Objects.push_front(node);

	}
	else if (node->VGetSphere() != nullptr && m_Props.m_BoundingBox.Contains(node->VGetSphere()) == Alpha_Contains)
	{
		bool found = false;
		//we will try to place the object into a child node. If we can't fit it in a child node, then we insert it into the current node object list.
		for (int a = 0; a < 8; a++)
		{
			//is the object contained within a child quadrant?
			if (childOctant[a].Contains(node->VGetSphere()) == Alpha_Contains)
			{
				if (m_ChildNodes[a] != nullptr)
					m_ChildNodes[a]->VAddChild(node);   //Add the item into that tree and let the child tree figure out what to do with it
				else
				{
					m_ChildNodes[a] = CreateNode(childOctant[a], node);   //create a new tree node with the item
					m_ActiveNodes |= (byte)(1 << a);
				}
				found = true;
			}
		}
		if(!found) m_Objects.push_front(node);
	}
	else
	{
		//either the item lies outside of the enclosed bounding box or it is intersecting it. Either way, we need to rebuild
		//the entire tree by enlarging the containing bounding box
		//BoundingBox enclosingArea = FindBox();
		BuildTree();
	}
}

//
// OctTreeNode::VRenderChildren					- Chapter 16, page 533
//
HRESULT OctTreeNode::VRenderChildren(Scene *pScene)
{
	if (m_TreeBuilt == true)
	{
		//go through and update every object in the current tree node
		for (LeafObjectList::const_iterator obj = m_Objects.begin(), end = m_Objects.end(); obj != end; ++obj)
		{
			if ((*obj)->VPreRender(pScene)==S_OK)
			{
				// You could short-circuit rendering
				// if an object returns E_FAIL from
				// VPreRender()

				// Don't render this node if you can't see it
				if ((*obj)->VIsVisible(pScene))
				{
					float alpha = (*obj)->VGet()->m_Material.GetAlpha();
					if (alpha==fOPAQUE)
					{
						(*obj)->VRender(pScene);
					}
					else if (alpha!=fTRANSPARENT)
					{
						// The object isn't totally transparent...
						AlphaSceneNode *asn = AC_NEW AlphaSceneNode;
						AC_ASSERT(asn);
						asn->m_pNode = *obj;
						asn->m_Concat = pScene->GetTopMatrix();
					
						Vec4 worldPos(asn->m_Concat.GetPosition());
					
						Mat4x4 fromWorld = pScene->GetCamera()->VGet()->FromWorld();

						Vec4 screenPos = fromWorld.Xform(worldPos);

						asn->m_ScreenZ = screenPos.z;

						pScene->AddAlphaSceneNode(asn);
					}
					(*obj)->VRenderChildren(pScene);
				}
			}
			(*obj)->VPostRender(pScene);
		}

		//recursively update any child nodes.
		for(int flags = m_ActiveNodes, index = 0; flags > 0; flags >>=1, index++)
		{
			if ((flags & 1) == 1)
			{
				if (m_ChildNodes[index]->VPreRender(pScene)==S_OK)
				{
					// You could short-circuit rendering
					// if an object returns E_FAIL from
					// VPreRender()

					// Don't render this node if you can't see it
					if (m_ChildNodes[index]->VIsVisible(pScene))
					{
						m_ChildNodes[index]->VRender(pScene);
						m_ChildNodes[index]->VRenderChildren(pScene);
					}
				}
				m_ChildNodes[index]->VPostRender(pScene);
			}
		}
	}
	else //if(m_TreeBuilt == false)
	{
		BuildTree();
		VOnRestore(pScene);
		//return S_OK;
	}
	return S_OK;
}

//
// SceneNode::VAddChild					- Chapter 16, page 535
////
//bool SceneNode::VAddChild(shared_ptr<ISceneNode> ikid)
//{
//	m_Children.push_back(ikid); 
//
//	shared_ptr<SceneNode> kid = static_pointer_cast<SceneNode>(ikid);
//
//	kid->m_pParent = this;					// [mrmike] Post-press fix - the parent was never set!
//
//	// The radius of the sphere should be fixed right here
//	Vec3 kidPos = kid->VGeterrrr()->ToWorld().GetPosition();
//	
//	// [mrmike] - Post-press fix. This was not correct! subtracting the parents's position from the kidPos
//	//            created a HUGE radius, depending on the location of the parent, which could be anywhere
//	//            in the game world.
//
//	//Vec3 dir = kidPos - m_Props.ToWorld().GetPosition();
//	//float newRadius = dir.Length() + kid->VGeterrrr()->Radius();
//	
//	float newRadius = kidPos.Length() + kid->VGeterrrr()->Radius(); 
//
//	if (newRadius > m_Props.m_Radius)
//		m_Props.m_Radius = newRadius;
//
//	return true; 
//}

//
// OctTreeNode::VRemoveChild					- not in the book
//
//   If an actor is destroyed it should be removed from the scene graph.
//   Generally the HumanView will recieve a message saying the actor has been
//   destroyed, and it will then call Scene::RemoveChild which will traverse 
//   the scene graph to find the child that needs removing.
//
bool OctTreeNode::VRemoveChild(ActorId id)
{
	if (m_TreeBuilt == true)
	{
		//go through and call VRemoveChild on every object in the current tree node
		//in GAMECODE4 he continued to attempt to delete actors even after he receive a true, possible multipl objects tied to on actor id?
		for (LeafObjectList::const_iterator obj = m_Objects.begin(), end = m_Objects.end(); obj != end; ++obj)
		{
			if((*obj)->VRemoveChild(id) == true)
				return true;
		}

		//recursively update any child nodes.
		for( int flags = m_ActiveNodes, index = 0; flags > 0; flags >>=1, index++)
		{
			if ((flags & 1) == 1)
			{
				if(m_ChildNodes[index]->VRemoveChild(id) == true)
					return true;
			}
		}
	}
	return false;
}

HRESULT OctTreeNode::VPick(Scene *pScene, RayCast *raycast) 
{
	for (LeafObjectList::const_iterator obj = m_Objects.begin(), end = m_Objects.end(); obj != end; ++obj)
	{
		HRESULT hr = (*obj)->VPick(pScene, raycast);

		if (hr==E_FAIL)
			return E_FAIL;
	}

	// TODO pass VPick on to all the childnodes, right now this will only check object contained in the root node of the octree

	return S_OK;
}

//
// OctTreeNode::ProcessesInsertions
//	Processes all pending insertions by inserting them into the tree.
//
void OctTreeNode::ProcessesInsertions()
{
	if (!m_TreeBuilt)
	{
		while (m_PendingInsertion.size() != 0)
		{
			m_Objects.push_front(m_PendingInsertion.front());
			m_PendingInsertion.pop();
		}

		BuildTree();
	}
	else
	{
		while (m_PendingInsertion.size() != 0)
		{
			VAddChild(m_PendingInsertion.front());
			m_PendingInsertion.pop();
		}
	}
	m_TreeReady = true;
}

//
// OctTreeNode::BuildTree()
//	Naively builds an oct tree from scratch.
//
void OctTreeNode::BuildTree()
{
	// Terminate the recursion if we're a leaf node
	if (m_Objects.size() <= 1)
		return;

	Vec3 dimensions = m_Props.m_BoundingBox.Dimensions();

	if (dimensions.Length() == 0.0f)
	{
		FindEnclosingCube();
		dimensions = m_Props.m_BoundingBox.Dimensions();
	}

	// Check to see if the dimensions of the box are greater than the minimum dimensions
	if (dimensions.x <= MIN_SIZE && dimensions.y <= MIN_SIZE && dimensions.z <= MIN_SIZE)
	{
		return;
	}

	// Create subdivided regions for each octant
	std::array<BoundingBox, 8> octant;
	octant[0] = BoundingBox(m_Props.m_BoundingBox.MinVec3(), m_Props.m_BoundingBox.m_Center);
	octant[1] = BoundingBox(Vec3(m_Props.m_BoundingBox.m_Center.x, m_Props.m_BoundingBox.MinVec3().y, m_Props.m_BoundingBox.MinVec3().z), Vec3(m_Props.m_BoundingBox.MaxVec3().x, m_Props.m_BoundingBox.m_Center.y, m_Props.m_BoundingBox.m_Center.z));
	octant[2] = BoundingBox(Vec3(m_Props.m_BoundingBox.m_Center.x, m_Props.m_BoundingBox.MinVec3().y, m_Props.m_BoundingBox.m_Center.z), Vec3(m_Props.m_BoundingBox.MaxVec3().x, m_Props.m_BoundingBox.m_Center.y, m_Props.m_BoundingBox.MaxVec3().z));
	octant[3] = BoundingBox(Vec3(m_Props.m_BoundingBox.MinVec3().x, m_Props.m_BoundingBox.MinVec3().y, m_Props.m_BoundingBox.m_Center.z), Vec3(m_Props.m_BoundingBox.m_Center.x, m_Props.m_BoundingBox.m_Center.y, m_Props.m_BoundingBox.MaxVec3().z));
	octant[5] = BoundingBox(Vec3(m_Props.m_BoundingBox.m_Center.x, m_Props.m_BoundingBox.m_Center.y, m_Props.m_BoundingBox.MinVec3().z), Vec3(m_Props.m_BoundingBox.MaxVec3().x, m_Props.m_BoundingBox.MaxVec3().y, m_Props.m_BoundingBox.m_Center.z));
	octant[6] = BoundingBox(m_Props.m_BoundingBox.m_Center, m_Props.m_BoundingBox.MaxVec3());
	octant[7] = BoundingBox(Vec3(m_Props.m_BoundingBox.MinVec3().y, m_Props.m_BoundingBox.m_Center.y, m_Props.m_BoundingBox.m_Center.y), Vec3(m_Props.m_BoundingBox.m_Center.x, m_Props.m_BoundingBox.MaxVec3().y, m_Props.m_BoundingBox.MaxVec3().z));

	// This will contain all of our objects which fit within each respective octant.
	std::array<LeafObjectList, 8> octList; 

	// This list contains all of the objects which got moved down the tree and can be delisted from this node.
	LeafObjectList delist;

	for (LeafObjectList::const_iterator obj = m_Objects.begin(), end = m_Objects.end(); obj != end; ++obj) 
	{
		if ((*obj)->VGetBox() != nullptr)//(*obj)->m_BoundingBox->MinVec3() != (*obj)->m_BoundingBox->MaxVec3())
		{
			for (int a = 0; a < 8; a++)
			{
				if (octant[a].Contains((*obj)->VGetBox()) == Alpha_Contains)
				{
					octList[a].push_front(*obj);
					delist.push_front(*obj);
					break;
				}
			}
		}
		else if ((*obj)->VGetSphere()->m_Radius != 0)
		{
			for (int a = 0; a < 8; a++)
			{
				if (octant[a].Contains(*(*obj)->VGetSphere()) == Alpha_Contains)
				{
					octList[a].push_front(*obj);
					delist.push_front(*obj);
					break;
				}
			}
		}
	}

	// Delist every moved object from this node.
	for (LeafObjectList::const_iterator obj = delist.begin(), end = delist.end(); obj != end; ++obj) 
	{
		m_Objects.remove(*obj);
	}

	// Create child nodes where there are items contained in the bounding region
	for (int a = 0; a < 8; a++)
	{
		if (octList[a].size() != 0)
		{
			m_ChildNodes[a] = CreateNode(octant[a], octList[a]);
			m_ActiveNodes |= (byte)(1 << a);
			m_ChildNodes[a]->BuildTree();
		}
	}

	m_TreeBuilt = true;
	m_TreeReady = true;
}

//
// OctTreeNode::FindEnclosingBox()
//	This finds the dimensions of the bounding box necessary to tightly enclose all items in the object list.
//
void OctTreeNode::FindEnclosingBox()
{
	Vec3 global_min = m_Props.m_BoundingBox.MinVec3();
	Vec3 global_max = m_Props.m_BoundingBox.MaxVec3();

    //go through all the objects in the list and find the extremes for their bounding areas.
    for (LeafObjectList::const_iterator it = m_Objects.begin(), end = m_Objects.end(); it != end; ++it) {
		
		Vec3 local_min;
		Vec3 local_max;

        if ((*it)->VGetBox() == nullptr) // (!(*it)->HasBounds)
        {
            //the object doesn't have any bounding regions associated with it, so we're going to skip it.
            //otherwise, we'll get stack overflow exceptions since we'd be creating an infinite number of nodes approaching zero.
            //continue;
            //throw new Exception("Every object in the octTree must have a bounding region!");
			AC_FATAL("Every object in the octTree must have a bounding region!");
        }

        if ((*it)->VGetBox() != nullptr && (*it)->VGetBox()->MaxVec3() != (*it)->VGetBox()->MinVec3())
        {
			local_min = (*it)->VGetBox()->MinVec3();
            local_max = (*it)->VGetBox()->MaxVec3();
        }

        if ((*it)->VGetSphere() != nullptr && (*it)->VGetSphere()->m_Radius != 0.0f)
        {
                    
            local_min = Vec3((*it)->VGetSphere()->m_Center.x - (*it)->VGetSphere()->m_Radius, 
				(*it)->VGetSphere()->m_Center.y - (*it)->VGetSphere()->m_Radius, 
                (*it)->VGetSphere()->m_Center.z - (*it)->VGetSphere()->m_Radius);
            local_max = Vec3((*it)->VGetSphere()->m_Center.x + (*it)->VGetSphere()->m_Radius,
                (*it)->VGetSphere()->m_Center.y + (*it)->VGetSphere()->m_Radius,
                (*it)->VGetSphere()->m_Center.z + (*it)->VGetSphere()->m_Radius);
        }

        if (local_min.x < global_min.x) global_min.x = local_min.x;
        if (local_min.y < global_min.y) global_min.y = local_min.y;
        if (local_min.z < global_min.z) global_min.z = local_min.z;

        if (local_max.x > global_max.x) global_max.x = local_max.x;
        if (local_max.y > global_max.y) global_max.y = local_max.y;
        if (local_max.z > global_max.z) global_max.z = local_max.z;
    }

	m_Props.m_BoundingBox = BoundingBox(global_min, global_max);
}

//
// OctTreeNode::FindEnclosingCube()
//	This finds the smallest enclosing cube which is a power of 2, for all objects in the list.
//
void OctTreeNode::FindEnclosingCube()
{
    FindEnclosingBox();

    //find the min offset from (0,0,0) and translate by it for a short while
	Vec3 offset = m_Props.m_BoundingBox.MinVec3() - Vec3();

	m_Props.m_BoundingBox = BoundingBox(m_Props.m_BoundingBox.MinVec3() += offset, m_Props.m_BoundingBox.MaxVec3() += offset);

    //find the nearest power of two for the max values
	int highX = (int)std::floor(std::max(std::max(m_Props.m_BoundingBox.MaxVec3().x, m_Props.m_BoundingBox.MaxVec3().y), m_Props.m_BoundingBox.MaxVec3().z));

    //see if we're already at a power of 2
    for (int bit = 0; bit < 32; bit++)
    {
        if (highX == 1 << bit)
        {
			m_Props.m_BoundingBox = BoundingBox(m_Props.m_BoundingBox.MinVec3() += offset, Vec3((float)highX, (float)highX, (float)highX) += offset);
            return;
        }
    }

    //gets the most significant bit value, so that we essentially do a Ceiling(X) with the 
    //ceiling result being to the nearest power of 2 rather than the nearest integer.
	int x = Math::MostSigBit(highX);     

	m_Props.m_BoundingBox = BoundingBox(m_Props.m_BoundingBox.MinVec3() += offset, Vec3((float)x, (float)x, (float)x) += offset);
}

////////////////////////////////////////////////////
// RootNode Implementation
////////////////////////////////////////////////////

//
// RootNode::RootNode					- Chapter 16, page 545
//
RootOctNode::RootOctNode()
: SceneNode(INVALID_ACTOR_ID, WeakBaseRenderComponentPtr(), RenderPass_0, &Mat4x4::g_Identity)
{
	m_Children.reserve(RenderPass_Last);

	shared_ptr<SceneNode> staticGroup(AC_NEW SceneNode(INVALID_ACTOR_ID,  WeakBaseRenderComponentPtr(),  RenderPass_Static, &Mat4x4::g_Identity));
	m_Children.push_back(staticGroup);	// RenderPass_Static = 0

	shared_ptr<OctTreeNode> actorGroup(AC_NEW OctTreeNode()); //SceneNode(INVALID_ACTOR_ID,  WeakBaseRenderComponentPtr(),  RenderPass_Actor, &Mat4x4::g_Identity));
	m_Children.push_back(actorGroup);	// RenderPass_Actor = 1

	shared_ptr<SceneNode> skyGroup(AC_NEW SceneNode(INVALID_ACTOR_ID,  WeakBaseRenderComponentPtr(),  RenderPass_Sky, &Mat4x4::g_Identity));
	m_Children.push_back(skyGroup);	// RenderPass_Sky = 2

	shared_ptr<SceneNode> invisibleGroup(AC_NEW SceneNode(INVALID_ACTOR_ID,  WeakBaseRenderComponentPtr(),  RenderPass_NotRendered, &Mat4x4::g_Identity));
	m_Children.push_back(invisibleGroup);	// RenderPass_NotRendered = 3
}

//
// RootOctNode::VAddChild					- Chapter 16, page 546
//
bool RootOctNode::VAddChild(shared_ptr<ISceneNode> kid)
{
	// The Root node has children that divide the scene graph into render passes.
	// Scene nodes will get added to these children based on the value of the
	// render pass member variable.

	RenderPass pass = kid->VGet()->RenderPass();
	if ((unsigned)pass >= m_Children.size() || !m_Children[pass])
	{
		AC_ASSERT(0 && _T("There is no such render pass"));
		return false;
	}

	return m_Children[pass]->VAddChild(kid);
}

//
// RootOctNode::VRemoveChild						- not described in the book 
//
//   Returns false if nothing was removed
//
bool RootOctNode::VRemoveChild(ActorId id)
{
	bool anythingRemoved = false;
	for(int i=RenderPass_0; i<RenderPass_Last; ++i)
	{
		if(m_Children[i]->VRemoveChild(id))
		{
			anythingRemoved = true;
		}
	}
	return anythingRemoved;
}

//
// RootOctNode::VRenderChildren					- Chapter 16, page 547
//
HRESULT RootOctNode::VRenderChildren(Scene *pScene)
{
	// This code creates fine control of the render passes.
	for (int pass = RenderPass_0; pass < RenderPass_Last; ++pass)
	{
		switch(pass)
		{
			case RenderPass_Static:
			case RenderPass_Actor:
				m_Children[pass]->VRenderChildren(pScene);
				break;

			case RenderPass_Sky:
			{
				shared_ptr<IRenderState> skyPass = pScene->GetRenderer()->VPrepareSkyBoxPass();
				m_Children[pass]->VRenderChildren(pScene);
				break;
			}
		}
	}

	return S_OK;
}