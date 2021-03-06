#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//William Montgomery Cone
	vector3 originPoint(0, (-1 * a_fHeight) / 2, 0);
	vector3 topPoint(0, originPoint.y + a_fHeight, 0);
	float angleDeg = 360 / (a_nSubdivisions * 1.0f);

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//Points for a tri on the bottom of the cone
		vector3 bPoint1 = originPoint;
		vector3 bPoint2(glm::cos(glm::radians(angleDeg * i)) * a_fRadius,  //Cos of current angle * radius
						originPoint.y,									   //Origin y
						glm::sin(glm::radians(angleDeg * i)) * a_fRadius); //Sin of current angle * radius
		vector3 bPoint3(glm::cos(glm::radians(angleDeg * (i + 1))) * a_fRadius, //Cos of next angle * radius
						originPoint.y,											//Origin y
						glm::sin(glm::radians(angleDeg * (i + 1))) * a_fRadius);//Sin of next angle * radius

		//Add bottom tri
		AddTri(bPoint1, bPoint2, bPoint3);

		//Add the tri that connects to that bottom tri and touches the top of the cone
		AddTri(topPoint, bPoint3, bPoint2);
	}
	
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//William Montgomery Cylinder
	vector3 originPoint(0, (-1 * a_fHeight)/2, 0);
	float angleDeg = 360 / (a_nSubdivisions * 1.0f);

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//Points for a tri on the bottom of the cylinder
		vector3 bPoint1 = originPoint;
		vector3 bPoint2(glm::cos(glm::radians(angleDeg * i)) * a_fRadius,  //Cos of current angle * radius
						originPoint.y,									   //Origin y
						glm::sin(glm::radians(angleDeg * i)) * a_fRadius); //Sin of current angle * radius

		vector3 bPoint3(glm::cos(glm::radians(angleDeg * (i + 1))) * a_fRadius, //Cos of next angle * radius
						originPoint.y,											//Origin y
						glm::sin(glm::radians(angleDeg * (i + 1))) * a_fRadius);//Sin of next angle * radius

		AddTri(bPoint1, bPoint2, bPoint3);

		//Points for same tri on the top of the cylinder (add height)
		vector3 tPoint1(originPoint.x,
						originPoint.y + a_fHeight,
						originPoint.z);

		vector3 tPoint2(bPoint2.x,
						bPoint2.y + a_fHeight,
						bPoint2.z);

		vector3 tPoint3(bPoint3.x,
						bPoint3.y + a_fHeight,
						bPoint3.z);

		AddTri(tPoint1, tPoint3, tPoint2);

		//Quad that connects these two tris
		AddQuad(bPoint3, bPoint2, tPoint3, tPoint2);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//William Montgomery Tube
	vector3 originPoint(0, (-1 * a_fHeight) / 2, 0);

	float angleDeg = 360 / (a_nSubdivisions * 1.0f);

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//Points for a quad on the bottom of the tube
		//Outer Radius
		vector3 bPoint1(glm::cos(glm::radians(angleDeg * i)) * a_fOuterRadius,  //Cos of current angle * outer radius
						originPoint.y,											//Origin y
						glm::sin(glm::radians(angleDeg * i)) * a_fOuterRadius);	//Sin of current angle * outer radius

		vector3 bPoint2(glm::cos(glm::radians(angleDeg * (i + 1))) * a_fOuterRadius, //Cos of next angle * outer radius
						originPoint.y,												 //Origin y
						glm::sin(glm::radians(angleDeg * (i + 1))) * a_fOuterRadius);//Sin of next angle *  outer radius

		//Inner Radius
		vector3 bPoint3(glm::cos(glm::radians(angleDeg * i)) * a_fInnerRadius,  //Cos of current angle * inner radius
						originPoint.y,														//Origin y
						glm::sin(glm::radians(angleDeg * i)) * a_fInnerRadius);				//Sin of current angle * inner radius

		vector3 bPoint4(glm::cos(glm::radians(angleDeg * (i + 1))) * a_fInnerRadius, //Cos of next angle * inner radius
						originPoint.y,												 //Origin y
						glm::sin(glm::radians(angleDeg * (i + 1))) * a_fInnerRadius);//Sin of next angle *  inner radius

		AddQuad(bPoint1, bPoint2, bPoint3, bPoint4);

		//Points for a quad on the top of the tube
		//Outer Radius
		vector3 tPoint1(bPoint1.x,
			bPoint1.y + a_fHeight,
			bPoint1.z);

		vector3 tPoint2(bPoint2.x,
			bPoint2.y + a_fHeight,
			bPoint2.z);
		//Inner Radius
		vector3 tPoint3(bPoint3.x,
			bPoint3.y + a_fHeight,
			bPoint3.z);

		vector3 tPoint4(bPoint4.x,
			bPoint4.y + a_fHeight,
			bPoint4.z);

		AddQuad(tPoint2, tPoint1, tPoint4, tPoint3);

		//Quads connecting top and bottom
		//Facing outward
		AddQuad(bPoint2, bPoint1, tPoint2, tPoint1);
		//Facing inward
		AddQuad(bPoint3, bPoint4, tPoint3, tPoint4);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	//if (a_nSubdivisions > 6) Commented out to allow more subdivisions for presentation
	//	a_nSubdivisions = 6;

	Release();
	Init();

	//William Montgomery Sphere

	vector3 originPoint(0, (-1 * a_fRadius) / 2, 0);
	float angleLat = 360 / (a_nSubdivisions * 1.0f); //Angle amount to change per subdivision for latitude
	float angleLong = 180 / (a_nSubdivisions * 1.0f); //Angle amount to change per subdivision for longitude

	for (int i = 0; i < a_nSubdivisions; i++) //Current latitude slice
	{
		for (int j = 0; j < a_nSubdivisions; j++) //Current longitude slice
		{
			vector3 bLeft(glm::sin(glm::radians(angleLong * j)) * glm::sin(glm::radians(angleLat * i)) * a_fRadius,   //Sin of current long * Sin of current lat * r
							glm::cos(glm::radians(angleLong * j)) * a_fRadius,										  //Cos of current long * r
							glm::sin(glm::radians(angleLong * j)) * glm::cos(glm::radians(angleLat * i)) * a_fRadius);//Sin of current long * Cos of current lat * r

			vector3 bRight(glm::sin(glm::radians(angleLong * (j + 1))) * glm::sin(glm::radians(angleLat * i)) * a_fRadius,  //Sin of next long * Sin of current lat * r
							glm::cos(glm::radians(angleLong * (j + 1))) * a_fRadius,										//Cos of next long * r
							glm::sin(glm::radians(angleLong * (j + 1))) * glm::cos(glm::radians(angleLat * i)) * a_fRadius);//Sin of next long * Cos of current lat * r

			vector3 tLeft(glm::sin(glm::radians(angleLong * j)) * glm::sin(glm::radians(angleLat * (i + 1))) * a_fRadius,   //Sin of current long * Sin of next lat * r
							glm::cos(glm::radians(angleLong * j)) * a_fRadius,												//Cos of current long * r
							glm::sin(glm::radians(angleLong * j)) * glm::cos(glm::radians(angleLat * (i + 1))) * a_fRadius);//Sin of current long * Cos of next lat * r

			vector3 tRight(glm::sin(glm::radians(angleLong * (j + 1))) * glm::sin(glm::radians(angleLat * (i + 1))) * a_fRadius,  //Sin of next long * Sin of next lat * r
							glm::cos(glm::radians(angleLong * (j + 1))) * a_fRadius,											  //Cos of next long * r
							glm::sin(glm::radians(angleLong * (j + 1))) * glm::cos(glm::radians(angleLat * (i + 1))) * a_fRadius);//Sin of next long * Cos of next lat * r
			
			AddQuad(bLeft, bRight, tLeft, tRight);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}