#include "MyRigidBody.h"

Simplex::MyRigidBody::MyRigidBody(MyMesh* a_pObject)
{
	if (a_pObject == nullptr)
		return;
	
	std::vector<vector3> pointsList = a_pObject->GetVertexList();

	if (pointsList.size() < 1)
		return;

	m_v3Min = pointsList[0];
	m_v3Max = pointsList[0];
	for (uint i = 1; i < pointsList.size(); i++)
	{
		if (pointsList[i].x < m_v3Min.x)
			m_v3Min.x = pointsList[i].x;
		if (pointsList[i].x > m_v3Max.x)
			m_v3Max.x = pointsList[i].x;

		if (pointsList[i].y < m_v3Min.y)
			m_v3Min.y = pointsList[i].y;
		if (pointsList[i].y > m_v3Max.y)
			m_v3Max.y = pointsList[i].y;

		if (pointsList[i].z < m_v3Min.z)
			m_v3Min.z = pointsList[i].z;
		if (pointsList[i].z > m_v3Max.z)
			m_v3Max.z = pointsList[i].z;
	}

	m_v3Center = m_v3Min + m_v3Max;
	m_v3Center = m_v3Center / 2.0f;
	float fRadius = glm::distance(m_v3Center, m_v3Max);

	m_BS = new Mesh();
	m_BS->GenerateIcoSphere(fRadius, 2, C_BLUE);

	m_BB = new Mesh();
	m_BB->GenerateCube(1.0f, C_WHITE);
}

void Simplex::MyRigidBody::Render(MyCamera* a_pCamera, matrix4 a_m4Model)
{
	float* matrix = new float[16];
	memcpy(matrix, glm::value_ptr(IDENTITY_M4), 16 * sizeof(float));
	vector3 v3Size = m_v3Max - m_v3Min;
	matrix4 resize = a_m4Model * glm::scale(IDENTITY_M4, v3Size);
	memcpy(matrix, glm::value_ptr(resize), 16 * sizeof(float));


	//m_BS->RenderWire(a_pCamera->GetProjectionMatrix(), a_pCamera->GetViewMatrix(), matrix, 1);
	m_BB->RenderWire(a_pCamera->GetProjectionMatrix(), a_pCamera->GetViewMatrix(), matrix, 1);
	SafeDelete(matrix);
}
