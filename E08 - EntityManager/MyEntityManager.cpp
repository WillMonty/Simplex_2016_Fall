#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
}
void MyEntityManager::Release(void)
{
	//Go through each entity in the entity list and delete it
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		MyEntity* toDelete = m_entityList[i];
		SafeDelete(toDelete);
	}
}
MyEntityManager* MyEntityManager::GetInstance()
{
	//If singleton hasn't been created yet, create it
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	//Delete the singleton pointer if it exists
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	//Go through entity list
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		//If found return the index
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID) { return i; }
	}

	//-1 indicates not found
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	if (m_entityList.size() == 0) { return nullptr; } //Make sure list isn't empty

	//If index out of bounds set it to last entry
	if (a_uIndex >= m_uEntityCount) { a_uIndex = m_uEntityCount - 1; }

	return m_entityList[a_uIndex]->GetModel();
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	MyEntity* e = MyEntity::GetEntity(a_sUniqueID);

	//If it exists get the model
	if (e != nullptr) { return e->GetModel(); }
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	if (m_entityList.size() == 0) { return nullptr; } //Make sure list isn't empty

	//If index out of bounds set it to last entry
	if (a_uIndex >= m_uEntityCount) { a_uIndex = m_uEntityCount - 1; }

	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	MyEntity* e = MyEntity::GetEntity(a_sUniqueID);

	//If it exists get the rigid body
	if (e != nullptr) { return e->GetRigidBody(); }

	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	if (m_entityList.size() == 0) { return IDENTITY_M4; } //If list is empty return identity matrix

	//If index out of bounds set it to last entry
	if (a_uIndex >= m_uEntityCount) { a_uIndex = m_uEntityCount - 1; }

	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	MyEntity* e = MyEntity::GetEntity(a_sUniqueID);

	//If it exists get the model matrix
	if (e != nullptr) { return e->GetModelMatrix(); }

	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	MyEntity* e = MyEntity::GetEntity(a_sUniqueID);

	//If e exists set its model matrix
	if (e != nullptr) { e->SetModelMatrix(a_m4ToWorld); }
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	if (m_entityList.size() == 0) { return; } //Make sure list isn't empty

	//If index out of bounds set it to last entry
	if (a_uIndex >= m_uEntityCount) { a_uIndex = m_uEntityCount - 1; }

	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld); //Set model matrix of entity at a_uIndex
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	//Check all collisions of entities in the list against each other
	//Run through list
	for (uint i = 0; i < m_uEntityCount - 1; i++)
	{
		for (uint j = 0; j < m_uEntityCount; j++)
		{
			if (j == i) { continue; }
			m_entityList[i]->IsColliding(m_entityList[j]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	MyEntity* e = new MyEntity(a_sFileName, a_sUniqueID);

	//If it initalized corrected
	if (e->IsInitialized())
	{
		//Add it to the list and update count
		m_entityList.push_back(e);
		m_uEntityCount = m_entityList.size();
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	if (m_entityList.size() == 0) { return; } //Make sure list isn't empty

	//Switch choice with last entry so it can be popped out
	//Check if choice is already the last entry
	if (a_uIndex != m_uEntityCount - 1)
	{
		std::swap(m_entityList[a_uIndex], m_entityList[m_uEntityCount - 1]);
	}

	//Delete last entry and then pop it
	MyEntity* e = m_entityList[m_uEntityCount - 1];
	SafeDelete(e);
	m_entityList.pop_back();

	m_uEntityCount--; //Update count
	return;
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	//Get index from ID and then call RemoveEntity(index)
	int removeIndex = GetEntityIndex(a_sUniqueID);
	RemoveEntity((uint)removeIndex);
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	if (m_entityList.size() == 0) { return ""; } //Make sure list isn't empty

	//If index out of bounds set it to last entry
	if (a_uIndex >= m_uEntityCount) { a_uIndex = m_uEntityCount - 1; }

	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	if (m_entityList.size() == 0) { return nullptr; } //Make sure list isn't empty

	//If index out of bounds set it to last entry
	if (a_uIndex >= m_uEntityCount) { a_uIndex = m_uEntityCount - 1; }

	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	//If the index is out of range add all of the entities in the list
	if (a_uIndex >= m_uEntityCount || a_uIndex < 0)
	{
		//Run through list
		for (uint i = 0; i < m_uEntityCount; i++)
		{
			m_entityList[i]->AddToRenderList(a_bRigidBody);
		}
	}
	else
	{
		//If in range add it to the entity list
		m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
	
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	MyEntity* e = MyEntity::GetEntity(a_sUniqueID);

	//If it exists add it to the render list
	if (e != nullptr) { e->AddToRenderList(a_bRigidBody); }
}