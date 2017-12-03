#include "MyOctant.h"
using namespace Simplex;

//Static variables
uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 0;
uint MyOctant::m_uIdealEntityCount = 0;

MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	m_uID = m_uOctantCount;
	m_uOctantCount++;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uMaxLevel = a_nMaxLevel;

	m_v3Center = ZERO_V3;
	m_fSize = 1.0f; //???

	m_pEntityMngr->GetInstance();
	m_pMeshMngr->GetInstance();

	ConstructTree(a_nMaxLevel);
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	m_uID = m_uOctantCount;
	m_uOctantCount++;

	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_pEntityMngr->GetInstance();
	m_pMeshMngr->GetInstance();
}

Simplex::MyOctant::MyOctant(Octant const & other)
{
	m_uID = m_uOctantCount;
	m_uOctantCount++;
}

MyOctant & Simplex::MyOctant::operator=(Octant const & other)
{
	return *m_pRoot; //CHANGE THIS
}

Simplex::MyOctant::~MyOctant(void)
{
	Release();
}

void Simplex::MyOctant::Swap(Octant & other)
{
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
	return false;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	if (m_uChildren != 0)
	{
		for (uint i = 0; i < m_uChildren; i++)
		{
			m_pChild[i]->Display(a_v3Color);
		}
	}
}

void Simplex::MyOctant::ClearEntityList(void)
{
	m_EntityList.clear();
}

void Simplex::MyOctant::Subdivide(void)
{
	if (m_uChildren != 0) //If there all children already tell those to subdivide
	{
		for (uint i = 0; i < m_uChildren; i++)
		{
			m_pChild[i]->Subdivide();
		}
	}
	else //No children means this is a leaf. Subdivide it.
	{
		float step = m_fSize * 0.25f; //Quater of the size of this square to step to when making children.
		vector3 offset = ZERO_V3; //Offset of new octant's center from this octant's center
		for (uint i = 0; i < 8; i++) //Make 8 octants
		{
			//Bitwise and operator to determine on what numbers to flip from positive to negative steps in the offset.
			//Creates all 8 centers for the 8 new octants
			offset.x = ((i & 1) ? step : -step);
			offset.y = ((i & 2) ? step : -step);
			offset.z = ((i & 4) ? step : -step);
			m_pChild[i] = new MyOctant(m_v3Center + offset, step);
		}
	}
}

MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild < 0 || a_nChild > 7)
	{
		return nullptr;
	}

	return m_pChild[a_nChild];
}

MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	if (m_uChildren > 0)
	{
		return true;
	}
	return false;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	if (m_EntityList.size() > a_nEntities)
	{
		return true;
	}
	return false;
}

void Simplex::MyOctant::KillBranches(void)
{
	if (m_uChildren == 0) //Return if no branches
	{
		return;
	}

	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->KillBranches();
		delete m_pChild[i];
	}
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	if (a_nMaxLevel == 0)
	{
		return; //Reached bottom level
	}

	//Use constructor with center to make 8 children. Pass level on - 1 to children.
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	if (m_uChildren == 0)
	{
		for (uint i = 0; i < m_EntityList.size(); i++)
		{
			//m_pEntityMngr->GetEntity(m_EntityList[i]). SetID Method???
		}
	}
	else
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
	m_pMeshMngr = nullptr;
	m_pEntityMngr = nullptr;
}

void Simplex::MyOctant::Init(void)
{
}

void Simplex::MyOctant::ConstructList(void)
{
}
