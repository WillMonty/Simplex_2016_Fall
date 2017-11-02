/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/11
----------------------------------------------*/
#ifndef __MYRIGIDBODY_H_
#define __MYRIGIDBODY_H_
#include "MyMesh.h"
#include "MyCamera.h"
#include "Definitions.h"

namespace Simplex
{

	class MyRigidBody
	{
	public:
		Mesh* m_BS = nullptr;
		Mesh* m_BB = nullptr;

		vector3 m_v3Min;
		vector3 m_v3Max;
		vector3 m_v3Center;

		MyRigidBody(MyMesh* a_pObject);
		void Render(MyCamera* a_pCamera, matrix4 a_m4Model);
	};

}
#endif
