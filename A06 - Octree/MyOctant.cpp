#include "MyOctant.h"
using namespace Simplex;

//Static variables
uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 0;
uint MyOctant::m_uIdealEntityCount = 0;

//Creates a brand new octree with a max level and ideal entity count
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	//Initialize default values first
	Init();

	//This is the root of the octree
	m_uOctantCount = 0;
	m_uID = m_uOctantCount;
	m_pRoot = this;
	m_uOctantCount++;

	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uMaxLevel = a_nMaxLevel;

	//////MyRigidBody has constuctor that takes list of points and makes a rigidbody around them. Use that with all the entities values to quickly find center and size of all entities in scene

	std::vector<vector3> entityMinMaxList; //List of all entities mins and maxes

	//Run through all entities in scene
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		entityMinMaxList.push_back(m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMinGlobal()); //Get entity -> get rigid body -> get min/max
		entityMinMaxList.push_back(m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMaxGlobal());
	}

	MyRigidBody* rbForAll = new MyRigidBody(entityMinMaxList); //Make rigid body out of all entities min and max

	//Get center
	m_v3Center = rbForAll->GetCenterLocal();

	//Get halfwidth to get size and min and max of octant
	vector3 rbHalfWidth = rbForAll->GetHalfWidth();

	float hwMax = std::max({ rbHalfWidth.x, rbHalfWidth.y, rbHalfWidth.z }); //Get largest dimension of halfwidth

	//Get size and min/max from max halfwidth extent
	m_fSize = hwMax * 2.0f;
	m_v3Min = m_v3Center - vector3(hwMax);
	m_v3Min = m_v3Center + vector3(hwMax);

	ConstructTree(m_uMaxLevel); //Make the tree from the passed level
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();

	m_uOctantCount++;

	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	//Use size to get min and max
	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);
}

Simplex::MyOctant::MyOctant(MyOctant const & other)
{
	m_pRoot, other.m_pRoot;
	m_pParent = other.m_pParent;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;
	m_fSize = other.m_fSize;

	m_lChild, other.m_lChild;
	m_uChildren = other.m_uChildren;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
}

MyOctant & Simplex::MyOctant::operator=(MyOctant const & other)
{
	if (this == &other)
	{
		return *this;
	}
	else //Just use swap if the two octants aren't already equivalent
	{
		//Re-initialize
		Release();
		Init();

		MyOctant toSwap(other);
		Swap(toSwap); //Swap other with this
	}
}

Simplex::MyOctant::~MyOctant(void)
{
	Release(); //Release already deletes everything
}

void Simplex::MyOctant::Swap(MyOctant & other)
{
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_pParent, other.m_pParent);
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);
	std::swap(m_fSize, other.m_fSize);

	std::swap(m_lChild, other.m_lChild);
	std::swap(m_pChild, other.m_pChild);
	std::swap(m_uChildren, other.m_uChildren);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++) //Swap all children
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}

float Simplex::MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Max;
}

vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Min;
}

bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	uint numEntities = m_pEntityMngr->GetEntityCount();

	//Don't check if if the index is out of range
	if (a_uRBIndex < 0 || a_uRBIndex >= numEntities) { return false; }
	
	//Get vectors of the other entity (global)
	MyRigidBody* otherRB = m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody();
	vector3 otherMin = otherRB->GetMinGlobal();
	vector3 otherMax = otherRB->GetMaxGlobal();

	//Use AABB collisions
	//X
	if (m_v3Min.x > otherMin.x) { return false; }
	if (m_v3Max.x < otherMax.x) { return false; }

	//Y
	if (m_v3Min.y > otherMin.y) { return false; }
	if (m_v3Max.y < otherMax.y) { return false; }

	//Z
	if (m_v3Min.z > otherMin.z) { return false; }
	if (m_v3Max.z < otherMax.z) { return false; }

	return true;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (m_uID == a_nIndex) //If this octant is being displayed
	{
		//Add wireframe cube of this octant to the mesh manager
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)),
															a_v3Color,
															RENDER_WIRE);
	}
	else
	{
		//Go down the tree and see if this is for children to display
		for (uint i = 0; i < m_uChildren; i++)
		{
			m_pChild[i]->Display(a_nIndex, a_v3Color);
		}
	}
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	//Go down tree and display
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_v3Color);
	}

	//Add wireframe cube of this octant to the mesh manager
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)),
		a_v3Color,
		RENDER_WIRE);
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_v3Color);
	}
}

void Simplex::MyOctant::ClearEntityList(void)
{
	//Clear all childrens' lists
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ClearEntityList();
	}
	m_EntityList.clear();
}

void Simplex::MyOctant::Subdivide(void)
{
	if (m_uChildren != 0) {return;} //Don't subdivide already divided octant

	if (m_uLevel == m_uMaxLevel) { return; } //Stop at max level

	float step = m_fSize / 4.0f; //Quater of the size of this square to step to when making children.
	vector3 offset = ZERO_V3; //Offset of new octant's center from this octant's center
	for (uint i = 0; i < 8; i++) //Make 8 octants
	{
		//Bitwise and operator to determine on what numbers to flip from positive to negative steps in the offset.
		//Creates all 8 centers for the 8 new octants
		offset.x = ((i & 1) ? step : -step);
		offset.y = ((i & 2) ? step : -step);
		offset.z = ((i & 4) ? step : -step);
		m_pChild[i] = new MyOctant(m_v3Center + offset, step);

		//Set variables from this octant
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
	}

	m_uChildren = 8;
}

MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild < 0 || a_nChild > 7){ return nullptr; } //Keep a_nChild in range

	return m_pChild[a_nChild];
}

MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	if (m_uChildren > 0){ return true; }
	return false;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	//Compare entity list size to parameter
	if (m_EntityList.size() > a_nEntities){ return true; }
	return false;
}

void Simplex::MyOctant::KillBranches(void)
{
	if (m_uChildren == 0) //Return if no branches
	{
		return;
	}

	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->KillBranches();
		delete m_pChild[i];
	}
	m_uChildren = 0;
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	if (a_nMaxLevel != 0) { return; } //Only do this to the root

	//Wipe tree
	ClearEntityList();
	KillBranches();
	m_lChild.clear();

	m_uMaxLevel = a_nMaxLevel;
	m_uOctantCount = 1;

	//Add ID's of octant's to contained entities
	AssignIDtoEntity();

	//Not necessary for assignment, but will be used elsewhere to determine how many subdivisions to make
	if (ContainsMoreThan(m_uIdealEntityCount)) 
	{
		Subdivide();
	}

	//Make the list of objects
	ConstructList();
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	if (IsLeaf())
	{
		//Run through all entities in the scene
		for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
		{
			if (IsColliding(i)) //If colliding add it to this octant's entity list
			{
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID); //Add "dimension" to entity with this octant's ID to keep what octant it is in.
			}
		}
	}
	else //If not a leaf keep going down the tree
	{
		for (uint i = 0; i < m_uChildren; i++)
		{
			m_pChild[i]->AssignIDtoEntity();
		}
	}
}

uint Simplex::MyOctant::GetOctantCount(void)
{
	return m_uOctantCount;
}

void Simplex::MyOctant::Release(void)
{
	if (m_uLevel == 0) { KillBranches(); } //Destroy all branches if this is the root

	//Clear entity lists and children
	m_EntityList.clear();
	m_lChild.clear();
	m_uChildren = 0;
}

//Set tree to initial state
void Simplex::MyOctant::Init(void)
{
	m_pRoot = nullptr;
	m_pParent = nullptr;

	m_uLevel = 0;
	m_uID = m_uOctantCount;

	m_fSize = 0.0f;
	m_v3Center = ZERO_V3;
	m_v3Min = ZERO_V3;
	m_v3Max = ZERO_V3;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}

//Build list of what octant's hold entities and give it to the root
void Simplex::MyOctant::ConstructList(void)
{
	//Add list to root's lChild list if there is an entity
	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}

	//Construct list for children too
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ConstructList();
	}

}
