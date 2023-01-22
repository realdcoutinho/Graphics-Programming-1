#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Render_Hardware.h"
#include "Render_Software.h"
#include "Scene.h"
#include <windows.h>


using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

void KeyBoardInput(bool loops)
{

}


int main(int argc, char* args[])
{

	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"Dual Rasterizer - ***Diogo de Moura Coutinho/2DAE07***",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;


	const auto pTimer = new Timer();
	const auto pScene = new Scene(pWindow);
	const auto pRenderHardware = new Render_Hardware(pWindow, pScene->GetCamera(), pScene->GetMesh());
	const auto pRenderSoftware = new Render_Software(pWindow, pScene->GetCamera(), pScene->GetMesh());

	bool isF1{ true };
	bool printFPS{ false };


	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;

	HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				//Test for a key
				


				//SHARED
				if (e.key.keysym.scancode == SDL_SCANCODE_F1)
				{
					if (isF1)
					{

						SetConsoleTextAttribute(h, 6);
						std::cout << "**(SHARED) Rasterizer Mode = Software" << '\n';
						isF1 = !isF1;
					}
					else
					{
						SetConsoleTextAttribute(h, 6);
						std::cout << "**(SHARED) Rasterizer Mode = Hardware" << '\n';
						isF1 = !isF1;
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
				{
					pRenderHardware->ToggleRotation();
					pRenderSoftware->ToggleRotation();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F9)
				{
					pRenderHardware->ToggleCullMode();
					pRenderSoftware->ToggleCullMode();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F10)
				{
					pRenderSoftware->ToggleUniformColor();
					pRenderHardware->ToggleUniformColor();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F11)
				{
					if(printFPS)
					{
						SetConsoleTextAttribute(h, 6);
						std::cout << "**(SHARED) Print FPS OFF" << '\n';
						printFPS = !printFPS;
					}
					else
					{
						SetConsoleTextAttribute(h, 6);
						std::cout << "**(SHARED) Print FPS ON" << '\n';
						printFPS = !printFPS;
					}
				}


				//HARDWARE
				if (e.key.keysym.scancode == SDL_SCANCODE_F3)
					pRenderHardware->ToggleFire();
				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
					pRenderHardware->ToggleSamplerState();

				//SOFTWARE
				if (e.key.keysym.scancode == SDL_SCANCODE_F5) //cycle shading
				{
					if (!isF1)
					{
						pRenderSoftware->ToggleShadingMode();
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F6) // toggle normal map
				{
					if (!isF1)
					{
						pRenderSoftware->ToggleNormalMap();
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F7) 
				{
					if (!isF1)
					{
						pRenderSoftware->ToggleDepthBuffer();
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F8)
				{
					if (!isF1)
					{
						pRenderSoftware->ToggleBoundingBox();
					}
				}
				break;
			default: ;
			}
		}

		SetConsoleTextAttribute(h, 15);
		//--------- Update ---------
			
		//	//***
		//	//MOUSE INPUT FOR SOFTWARE
		//pRenderSoftware->Update(pTimer);
		//pRenderHardware->Update(pTimer);
		
			//***
			//MOUSE INPUT FOR HARDWARE
		pRenderHardware->Update(pTimer);
		pRenderSoftware->Update(pTimer);



		//--------- Render ---------
		if (isF1)
		{
			pRenderHardware->Render();
		}
		else
		{
			pRenderSoftware->Render();
		}

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			if (printFPS)
			{
				SetConsoleTextAttribute(h, 8);
				std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
			}
		}
	}
	pTimer->Stop();

	SetConsoleTextAttribute(h, 15);
	//Shutdown "framework"
	delete pRenderHardware;
	delete pRenderSoftware;
	delete pScene;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}