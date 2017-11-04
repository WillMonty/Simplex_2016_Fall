#include "MyCamera.h"
using namespace Simplex;

//William Montgomery Methods
void Simplex::MyCamera::MoveForward(float distance)
{
	m_v3Position += forward * distance;
	m_v3Target += forward * distance;
	above += forward * distance;
	
	//Change direction vectors now that camera has moved
	up = glm::normalize(above - m_v3Position);
	forward = glm::normalize(m_v3Target - m_v3Position);
	right = glm::normalize(glm::cross(forward, up));

	CalculateProjectionMatrix(); //Recaclulate projection
}

void Simplex::MyCamera::MoveLeftRight(float distance)
{
	m_v3Position += right * distance;
	m_v3Target += right * distance;
	above += right * distance;

	//Change direction vectors now that camera has moved
	up = glm::normalize(above - m_v3Position);
	forward = glm::normalize(m_v3Target - m_v3Position);
	right = glm::normalize(glm::cross(forward, up));

	CalculateProjectionMatrix(); //Recaclulate projection
}

void Simplex::MyCamera::Pitch(float degree)
{
	float angleRad = glm::radians(degree);

	pitchYawRoll.x += angleRad; //Keep track of pitch rotation

	forward = glm::normalize(forward * glm::cos(angleRad) + up * glm::sin(angleRad));

	up = glm::cross(forward, right);
}

void Simplex::MyCamera::Yaw(float degree)
{
	float angleRad = glm::radians(degree);

	pitchYawRoll.y += angleRad; //Keep track of yaw rotation

	forward = glm::normalize(forward * glm::cos(angleRad) - right * glm::sin(angleRad));

	right = glm::cross(forward, up);
}

////////////////////////////////////////////////////////////////////

//Accessors
void Simplex::MyCamera::SetPosition(vector3 a_v3Position) { m_v3Position = a_v3Position; }

void Simplex::MyCamera::SetTarget(vector3 a_v3Target) { m_v3Target = a_v3Target; }

void Simplex::MyCamera::SetUp(vector3 a_v3Up) { up = a_v3Up; }

void Simplex::MyCamera::SetPerspective(bool a_bPerspective) { m_bPerspective = a_bPerspective; }

void Simplex::MyCamera::SetFOV(float a_fFOV) { m_fFOV = a_fFOV; }

void Simplex::MyCamera::SetResolution(vector2 a_v2Resolution) { m_v2Resolution = a_v2Resolution; }

void Simplex::MyCamera::SetNearFar(vector2 a_v2NearFar) { m_v2NearFar = a_v2NearFar; }

void Simplex::MyCamera::SetHorizontalPlanes(vector2 a_v2Horizontal) { m_v2Horizontal = a_v2Horizontal; }

void Simplex::MyCamera::SetVerticalPlanes(vector2 a_v2Vertical) { m_v2Vertical = a_v2Vertical; }

matrix4 Simplex::MyCamera::GetProjectionMatrix(void) { return m_m4Projection; }

matrix4 Simplex::MyCamera::GetViewMatrix(void) { CalculateViewMatrix(); return m_m4View; }

Simplex::MyCamera::MyCamera()
{
	Init(); //Init the object with default values
}

Simplex::MyCamera::MyCamera(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	Init(); //Initialize the object
	SetPositionTargetAndUp(a_v3Position, a_v3Target, a_v3Upward); //set the position, target and up
}

Simplex::MyCamera::MyCamera(MyCamera const& other)
{
	m_v3Position = other.m_v3Position;
	m_v3Target = other.m_v3Target;
	up = other.up;

	m_bPerspective = other.m_bPerspective;

	m_fFOV = other.m_fFOV;

	m_v2Resolution = other.m_v2Resolution;
	m_v2NearFar = other.m_v2NearFar;

	m_v2Horizontal = other.m_v2Horizontal;
	m_v2Vertical = other.m_v2Vertical;

	m_m4View = other.m_m4View;
	m_m4Projection = other.m_m4Projection;
}

MyCamera& Simplex::MyCamera::operator=(MyCamera const& other)
{
	if (this != &other)
	{
		Release();
		SetPositionTargetAndUp(other.m_v3Position, other.m_v3Target, other.up);
		MyCamera temp(other);
		Swap(temp);
	}
	return *this;
}

void Simplex::MyCamera::Init(void)
{
	ResetCamera();
	CalculateProjectionMatrix();
	CalculateViewMatrix();
	//No pointers to initialize here
}

void Simplex::MyCamera::Release(void)
{
	//No pointers to deallocate yet
}

void Simplex::MyCamera::Swap(MyCamera & other)
{
	std::swap(m_v3Position, other.m_v3Position);
	std::swap(m_v3Target, other.m_v3Target);
	std::swap(up, other.up);

	std::swap(m_bPerspective, other.m_bPerspective);

	std::swap(m_fFOV, other.m_fFOV);

	std::swap(m_v2Resolution, other.m_v2Resolution);
	std::swap(m_v2NearFar, other.m_v2NearFar);

	std::swap(m_v2Horizontal, other.m_v2Horizontal);
	std::swap(m_v2Vertical, other.m_v2Vertical);

	std::swap(m_m4View, other.m_m4View);
	std::swap(m_m4Projection, other.m_m4Projection);
}

Simplex::MyCamera::~MyCamera(void)
{
	Release();
}

void Simplex::MyCamera::ResetCamera(void)
{
	m_v3Position = vector3(0.0f, 0.0f, 10.0f); //Where my camera is located
	m_v3Target = vector3(0.0f, 0.0f, 0.0f); //What I'm looking at
	up = vector3(0.0f, 1.0f, 0.0f); //What is up

	m_bPerspective = true; //perspective view? False is Orthographic

	m_fFOV = 45.0f; //Field of View

	m_v2Resolution = vector2(1280.0f, 720.0f); //Resolution of the window
	m_v2NearFar = vector2(0.001f, 1000.0f); //Near and Far planes

	m_v2Horizontal = vector2(-5.0f, 5.0f); //Ortographic horizontal projection
	m_v2Vertical = vector2(-5.0f, 5.0f); //Ortographic vertical projection

	CalculateProjectionMatrix();
	CalculateViewMatrix();
}

void Simplex::MyCamera::SetPositionTargetAndUp(vector3 a_v3Position, vector3 a_v3Target, vector3 a_v3Upward)
{
	m_v3Position = a_v3Position;
	m_v3Target = a_v3Target;
	up = a_v3Position + a_v3Upward;
	CalculateProjectionMatrix();
}

void Simplex::MyCamera::CalculateViewMatrix(void)
{
	//William Montgomery View Matrix
	//Calculate rotation matrix from yaw and pitch
	//Initialize matrices to identity
	matrix4 pitchMat = IDENTITY_M4;
	matrix4 yawMat = IDENTITY_M4;

	//Rotate matrices according to stored angles
	pitchMat = glm::rotate(pitchMat, pitchYawRoll.x, vector3(1.0f, 0.0f, 0.0f));
	yawMat = glm::rotate(yawMat, pitchYawRoll.y, glm::vec3(0.0f, 1.0f, 0.0f));

	//Combine for rotation matrix
	matrix4 rotate = pitchMat * yawMat;

	//Get translation vector from what camera is looking at
	matrix4 translate = glm::translate(IDENTITY_M4, -1.0f * m_v3Target);

	//Calculate the view matrix
	m_m4View = rotate * translate;
}

void Simplex::MyCamera::CalculateProjectionMatrix(void)
{
	//perspective
	float fRatio = m_v2Resolution.x / m_v2Resolution.y;
	if (m_bPerspective)
	{
		m_m4Projection = glm::perspective(m_fFOV, fRatio, m_v2NearFar.x, m_v2NearFar.y);
	}
	else
	{
		m_m4Projection = glm::ortho(	m_v2Horizontal.x * fRatio, m_v2Horizontal.y * fRatio, //horizontal
										m_v2Vertical.x, m_v2Vertical.y, //vertical
										m_v2NearFar.x, m_v2NearFar.y); //near and far
	}
}
