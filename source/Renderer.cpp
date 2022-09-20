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

		float screenWidth	{640.0f};
		float screenHeigh	{480.0f};
		float aspectRatio	{ screenWidth / screenHeigh };

		float cX{ (((2 * (px + 0.5f)) / screenWidth) - 1) * aspectRatio };
		float cY{ 1 - (2 * py / screenHeigh) };


		Vector3 rayDirection{ cX, cY, 1 };
		Vector3 normalizedRayDirection{ rayDirection.Normalized() };

		Vector3 cameraOrigin{ 0, 0, 0 };

		Ray hitRay{ cameraOrigin, normalizedRayDirection };
		ColorRGB finalColor{ normalizedRayDirection.x, normalizedRayDirection.y, normalizedRayDirection.z };

		m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
			static_cast<uint8_t>(finalColor.r * 255),
			static_cast<uint8_t>(finalColor.g * 255),
			static_cast<uint8_t>(finalColor.b * 255));
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
