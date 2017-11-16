#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
std::string MyRigidBody::textSAT = "SAT not yet called";

void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if(SAT(a_pOther) != eSATResults::SAT_NONE)
			bColliding = false;// reset to false
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{	
	vector3 axes[3]; //Local axes of this object
	vector3 otherAxes[3]; //Local axes of the other object

	axes[0] = vector3(m_m4ToWorld * vector4(AXIS_X, 0));
	axes[1] = vector3(m_m4ToWorld * vector4(AXIS_Y, 0));
	axes[2] = vector3(m_m4ToWorld * vector4(AXIS_Z, 0));

	otherAxes[0] = vector3(a_pOther->m_m4ToWorld * vector4(AXIS_X, 0));
	otherAxes[1] = vector3(a_pOther->m_m4ToWorld * vector4(AXIS_Y, 0));
	otherAxes[2] = vector3(a_pOther->m_m4ToWorld * vector4(AXIS_Z, 0));

	//Variables holding radii of each object for individual tests
	float radius;
	float otherRadius;

	//Half width extents for this object and other object
	vector3 halfW = m_v3HalfWidth; //Variable solely shorthand for this object
	vector3 otherHalfW = a_pOther->m_v3HalfWidth;

	glm::mat3 rotToThis; //Rotation matrix taking other object and putting it in this object's orientation
	glm::mat3 AbsRot; //Aformentioned rotation matrix with "epsilon" term added in

	//Find Rotation matrix taking other object's rotation to this object's space
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			rotToThis[i][j] = glm::dot(axes[i], otherAxes[j]);
		}
	}

	//Find vector to place both object's global centers at same point
	vector3 centerV = a_pOther->GetCenterGlobal() - GetCenterGlobal();
	//Take that vector to this object's coordinate frame
	centerV = vector3(glm::dot(centerV, axes[0]), glm::dot(centerV, axes[1]), glm::dot(centerV, axes[2]));

	//Add epsilon term to rotation matrix prevent issues when edges are parallel causing the cross product to be null
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			AbsRot[i][j] = glm::abs(rotToThis[i][j]) + FLT_EPSILON;
		}
	}

	//Test for AX, AY, AZ
	for (int i = 0; i < 3; i++) 
	{
		radius = halfW[i];
		otherRadius = otherHalfW[0] * AbsRot[i][0] + otherHalfW[1] * AbsRot[i][1] + otherHalfW[2] * AbsRot[i][2]; //Get otherRadius from properly rotated half widths
		
		//If the absolute value of the center to center vector on (AXIS) is greater than the radius summed with the other radius on (AXIS) then the objects are seperated
		if (glm::abs(centerV[i]) > radius + otherRadius) 
		{ 
			//Switch statement for i is messy but is better than copying the exact same test 6 times for A and B with switched out eSATResults enums.
			switch (i)
			{
				case 0:
					textSAT = "AX";
					return eSATResults::SAT_AX;
					break;
				case 1:
					textSAT = "AY";
					return eSATResults::SAT_AY;
					break;
				case 2:
					textSAT = "AZ";
					return eSATResults::SAT_AZ;
					break;
			}

		}
	}

	//Test for BX, BY, BZ
	for (int i = 0; i < 3; i++) 
	{
		radius = halfW[0] * AbsRot[0][i] + halfW[1] * AbsRot[1][i] + halfW[2] * AbsRot[2][i];
		otherRadius = otherHalfW[i];

		if (glm::abs(centerV[i]) > radius + otherRadius)
		{
			switch (i)
			{
			case 0:
				textSAT = "BX";
				return eSATResults::SAT_BX;
				break;
			case 1:
				textSAT = "BY";
				return eSATResults::SAT_BY;
				break;
			case 2:
				textSAT = "BZ";
				return eSATResults::SAT_BZ;
				break;
			}

		}
	}

	//Cross products
	//AX x BX
	//The halfwidth components in the two axes used in this cross summed together AFTER proper projection onto the plane of this cross
	radius = halfW[1] * AbsRot[2][0] + halfW[2] * AbsRot[1][0];
	otherRadius = otherHalfW[1] * AbsRot[0][2] + otherHalfW[2] * AbsRot[0][1];
	//If the absolute value of the center to center vector on the plane of the current cross is greater than the sum of the radii of the objects projected to the same plane...
	//Then they are seperated
	if (glm::abs(centerV[2] * rotToThis[1][0] - centerV[1] * rotToThis[2][0]) > radius + otherRadius) 
	{
		textSAT = "AXxBX";
		return eSATResults::SAT_AXxBX; 
	}

	//AX x BY
	radius = halfW[1] * AbsRot[2][1] + halfW[2] * AbsRot[1][2];
	otherRadius = otherHalfW[0] * AbsRot[0][2] + otherHalfW[2] * AbsRot[0][0];
	if (glm::abs(centerV[2] * rotToThis[1][1] - centerV[1] * rotToThis[2][1]) > radius + otherRadius)
	{
		textSAT = "AXxBY";
		return eSATResults::SAT_AXxBY;
	}

	//AX x BZ
	radius = halfW[1] * AbsRot[2][2] + halfW[2] * AbsRot[1][2];
	otherRadius = otherHalfW[0] * AbsRot[0][1] + otherHalfW[1] * AbsRot[0][0];
	if (glm::abs(centerV[2] * rotToThis[1][2] - centerV[1] * rotToThis[2][2]) > radius + otherRadius)
	{
		textSAT = "AXxBZ";
		return eSATResults::SAT_AXxBZ;
	}

	//AY x BX
	radius = halfW[0] * AbsRot[2][1] + halfW[2] * AbsRot[0][1];
	otherRadius = otherHalfW[1] * AbsRot[1][2] + otherHalfW[2] * AbsRot[1][1];
	if (glm::abs(centerV[0] * rotToThis[2][0] - centerV[2] * rotToThis[0][0]) > radius + otherRadius)
	{
		textSAT = "AYxBX";
		return eSATResults::SAT_AYxBX;
	}

	//AY x BY
	radius = halfW[0] * AbsRot[2][1] + halfW[2] * AbsRot[0][1];
	otherRadius = otherHalfW[0] * AbsRot[1][2] + otherHalfW[2] * AbsRot[1][0];
	if (glm::abs(centerV[0] * rotToThis[2][1] - centerV[2] * rotToThis[0][1]) > radius + otherRadius)
	{
		textSAT = "AYxBY";
		return eSATResults::SAT_AYxBY;
	}

	//AY x BZ
	radius = halfW[0] * AbsRot[2][2] + halfW[2] * AbsRot[0][2];
	otherRadius = otherHalfW[0] * AbsRot[1][1] + otherHalfW[1] * AbsRot[1][0];
	if (glm::abs(centerV[0] * rotToThis[2][2] - centerV[2] * rotToThis[0][2]) > radius + otherRadius)
	{
		textSAT = "AYxBZ";
		return eSATResults::SAT_AYxBZ;
	}

	//AZ x BX
	radius = halfW[0] * AbsRot[1][0] + halfW[1] * AbsRot[0][0];
	otherRadius = otherHalfW[1] * AbsRot[2][2] + otherHalfW[2] * AbsRot[2][1];
	if (glm::abs(centerV[1] * rotToThis[0][0] - centerV[0] * rotToThis[1][0]) > radius + otherRadius)
	{
		textSAT = "AZxBX";
		return eSATResults::SAT_AZxBX;
	}

	//AZ x BY
	radius = halfW[0] * AbsRot[1][1] + halfW[1] * AbsRot[0][1];
	otherRadius = otherHalfW[0] * AbsRot[2][2] + otherHalfW[2] * AbsRot[2][0];
	if (glm::abs(centerV[1] * rotToThis[0][1] - centerV[0] * rotToThis[1][1]) > radius + otherRadius)
	{
		textSAT = "AZxBY";
		return eSATResults::SAT_AZxBY;
	}

	//AZ x BZ
	radius = halfW[0] * AbsRot[1][2] + halfW[1] * AbsRot[0][2];
	otherRadius = otherHalfW[0] * AbsRot[2][1] + otherHalfW[1] * AbsRot[2][0];
	if (glm::abs(centerV[1] * rotToThis[0][2] - centerV[0] * rotToThis[1][2]) > radius + otherRadius)
	{
		textSAT = "AZxBY";
		return eSATResults::SAT_AZxBY;
	}

	//there is no axis test that separates this two objects
	return eSATResults::SAT_NONE;
}