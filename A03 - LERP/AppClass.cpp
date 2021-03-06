#include "AppClass.h"
void Application::InitVariables(void)
{
	m_sProgrammer = "William Montgomery - wgm8510@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		uColor -= static_cast<uint>(decrements); //decrease the wavelength

		//New vector for this orbit
		std::vector<vector3> currOrbit;

		float anglePerSide = 360.0f / i; //Get angle amount per exterior angle of n-gon
		
		//Calculate and add points to this orbit
		for (int j = 0; j < i; j++)
		{
			//fSize used as something like a radius
			float x = glm::cos(glm::radians(anglePerSide * j)) * fSize; //Cos of current angle * fSize
			float y = glm::sin(glm::radians(anglePerSide * j)) * fSize; //sin of current angle * fSize
			currOrbit.push_back(vector3(x, y, 0.0f)); //Add vector to this orbit
		}

		orbitStops.push_back(currOrbit);

		fSize += 0.5f; //increment the size for the next orbit
	}
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	//Set up timer
	static float fTimer = 0; //Store the new timer's time
	static uint uClock = m_pSystem->GenClock(); //Generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //Get the delta time for that timer

	static uint currTrip = 0; //Current trip all of the orbits are on
	float tripTime = 1.0f; //Time it takes to get from point to point

	float tripPercent = MapValue(fTimer, 0.0f, tripTime, 0.0f, 1.0f); //Get percentage of current trip' completion by mapping value between 0.0 and 1.0

	//Draw shapes
	for (uint i = 0; i < m_uOrbits; i++)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));

	   //Calculate the current position
		vector3 start = orbitStops[i][currTrip % orbitStops[i].size()]; //Sub vector at current point
		vector3 end = orbitStops[i][(currTrip + 1) % orbitStops[i].size()]; //Sub vector at next point
		vector3 v3CurrentPos = glm::lerp(start, end, tripPercent); //LERP position

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.2)), C_WHITE);
	}

	if (tripPercent >= tripTime) //If the trip is complete
	{
		currTrip++;
		fTimer = m_pSystem->GetDeltaTime(uClock); //Reset clock
	}

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
	//release GUI
	ShutdownGUI();
}