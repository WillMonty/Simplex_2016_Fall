#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "William Montgomery - wgm8510@rit.edu";

	//Set shape string (0's are holes 1's are boxes)
	shapeStr =  "00100000100"
				"00010001000"
				"00111111100"
				"01101110110"
				"11111111111"
				"10111111101"
				"10100000101"
				"00011011000";

	//Allows array to be made based on 1's in the drawing above.
	int oneCount = std::count(shapeStr.begin(), shapeStr.end(), '1');
	meshArr = new MyMesh[oneCount];

	//Initialize mesh array 
	for (int i = 0; i < oneCount; i++)
	{
		meshArr[i].GenerateCube(1.0f, C_BLACK);
	}

	//Set initial x position of shape
	currXPos = -10.0f;

	//The max width of a row in the shape (If changing the drawing change this to match it's length)
	rowLength = 11;

	//Max height the shape can be on screen
	maxY = 6.0f;

	//The amount per frame the shape moves
	moveSpeed = 0.08f;

}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	matrix4 moveMat = IDENTITY_M4;

	float distFromTop = 0.0f; //Distance from the top of the shape
	float distFromLeft = 0.0f; //Distance from the left most part of the shape
	int currMesh = 0;
	for (int i = 0; i < shapeStr.size(); i++)
	{
		if (i % rowLength == 0) //New row?
		{
			distFromTop += 1.0f;
			distFromLeft = 0.0f;
		}

		if (shapeStr[i] == '1') //If a box should be rendered in this spot
		{
			moveMat = glm::translate(IDENTITY_M4, vector3(currXPos + distFromLeft, maxY - distFromTop, 0));
			meshArr[currMesh].Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), moveMat);

			currMesh++;
		}

		distFromLeft += 1.0f;
	}

	currXPos += moveSpeed; //Move entire shape by moveSpeed

	//m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), ToMatrix4(m_qArcBall));
	//m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3( 3.0f, 0.0f, 0.0f)));
		
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	/*
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	*/
	//SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}