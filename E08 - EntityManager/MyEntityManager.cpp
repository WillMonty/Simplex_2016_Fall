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
	for (int i = 0; i < m_uEntityCount; i++)
	{
		MyEntity* toDelete = m_entityList[i];
		SafeDelete(toDelete);
	}
}
MyEntityManager* MyEntityManager::GetInstance()
{
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
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	return nullptr;
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	return IDENTITY_M4;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	//Get correct entity from passed ID
	MyEntity* e = MyEntity::GetEntity(a_sUniqueID);

	//If e now exists set its model matrix
	if (e != nullptr) { e->SetModelMatrix(a_m4ToWorld); }
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	//Keep index in bounds
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

}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	MyEntity* toAdd = new MyEntity(a_sFileName, a_sUniqueID); //Create temporary entity to add

	//If it initalized corrected
	if (toAdd->IsInitialized())
	{
		m_entityList.push_back(toAdd);
		m_uEntityCount = m_entityList.size();
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{

}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{

}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	return "";
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	return nullptr;
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	//If the index is out of range add all of the entities in the list
	if (a_uIndex >= m_uEntityCount || a_uIndex < 0)
	{
		//Run through list
		for (int i = 0; i < m_uEntityCount; i++)
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
	MyEntity* eAdd = MyEntity::GetEntity(a_sUniqueID);

	//If it exists add it to the render list
	if (eAdd != nullptr) { eAdd->AddToRenderList(a_bRigidBody); }
}