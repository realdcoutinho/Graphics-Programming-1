//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"
#include <iostream>

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			//float gradient = px / static_cast<float>(m_Width);
			//gradient += py / static_cast<float>(m_Width);
			//gradient /= 2.0f;
			//ColorRGB finalColor{ gradient, gradient, gradient };
			////Update Color in Buffer
			//finalColor.MaxToOne();


		//variables for the aspect ration
		float screenWidth	{640.0f}; // m_Width
		float screenHeigh	{480.0f}; // m_Height
		//as[ect ration calculation and variable
		float aspectRatio	{ screenWidth / screenHeigh };

		//calculating camera posX and posY based on the given formula
		float cameraPosX{ (((2 * (px + 0.5f)) / screenWidth) - 1) * aspectRatio };
		float cameraPosY{ 1 - (2 * py / screenHeigh) };


		//creating a loof vector for the camera
		Vector3 rayDirection{ cameraPosX, cameraPosY, 1 };
		//normalazing said vector
		Vector3 normalizedRayDirection{ rayDirection.Normalized() };
		
		//CameraOrigin is where the look vector begins
		Vector3 cameraOrigin{ 0, 0, 0 };

		Ray hitRay{ cameraOrigin, normalizedRayDirection };
		ColorRGB finalColor{ normalizedRayDirection.x, normalizedRayDirection.y, normalizedRayDirection.z };


		m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
			static_cast<uint8_t>(finalColor.r * 255),
			static_cast<uint8_t>(finalColor.g * 255),
			static_cast<uint8_t>(finalColor.b * 255));

		Ray viewRay(cameraOrigin, normalizedRayDirection);
		ColorRGB finalColorSphere{255, 0, 0};
		HitRecord closestHit{};


	
		Sphere testSphere{ {0.0f, 0.0f, 100.0f}, 50.0f, 1 };

		GeometryUtils::HitTest_Sphere(testSphere, viewRay, closestHit);

			if (closestHit.didHit)
			{
				finalColorSphere = materials[closestHit.didHit]->Shade();
			}
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
