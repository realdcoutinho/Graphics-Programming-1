#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Render_Hardware.h"
#include "Render_Software.h"
#include <windows.h>


using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

void PrintInitialMessage()
{
	HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(h, 6);
	std::cout << "[Key Bindings - SHARED]" << '\n';
	std::cout << "	[F1] Toggle Rasterizer Mode(HARDWARE / SOFTWARE)"  << '\n';
	std::cout << "	[F2]  Toggle Vehicle Rotation(ON / OFF)" << '\n';
	std::cout << "	[F9]  Cycle CullMode(BACK / FRONT / NONE) "<< '\n';
	std::cout << "	[F10] Toggle Uniform ClearColor(ON / OFF)" << '\n';
	std::cout << "	[F11] Toggle Print FPS(ON / OFF)" << '\n';

	SetConsoleTextAttribute(h, 2);
	std::cout << "[Key Bindings - HARDWARE]" << '\n';
	std::cout << "	[F3] Toggle FireFX(ON / OFF)" << '\n';
	std::cout << "	[F4] Cycle Sampler State(POINT / LINEAR / ANISOTROPIC)" << '\n';

	SetConsoleTextAttribute(h, 5);
	std::cout << "[Key Bindings - SOFTWARE]" << '\n';
	std::cout << "	[F5] Cycle Shading Mode(COMBINED / OBSERVED_AREA / DIFFUSE / SPECULAR)" << '\n';
	std::cout << "	[F6] Toggle NormalMap(ON / OFF)" << '\n';
	std::cout << "	[F7] Toggle DepthBuffer Visualization(ON / OFF)" << '\n';
	std::cout << "	[F8] Toggle BoundingBox Visualization(ON / OFF)" << '\n';
	std::cout << '\n';

	SetConsoleTextAttribute(h, 15);
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

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderHardware = new Render_Hardware(pWindow);
	const auto pRenderSoftware = new Render_Software(pWindow);

	bool isF1{ true };
	bool printFPS{ false };


	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;

	PrintInitialMessage();

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

		//--------- Update ---------

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

	//Shutdown "framework"
	delete pRenderHardware;
	delete pRenderSoftware;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}