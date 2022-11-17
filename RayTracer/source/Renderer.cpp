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
#include <future> //ASYNC
#include <ppl.h>

using namespace dae;

//#define ASYNC
#define PARALLEL_FOR
//none of the above means they will be working with synchronous logic (no threading)



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
	camera.CalculateCameraToWorld();

	float aspectRatio{ static_cast<float>(m_Width) / static_cast<float>(m_Height) };
	float FOV{ camera.fov };

	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const uint32_t numPixels = m_Width * m_Height;
	
	const float offset{ 0.0001f };



#if defined(ASYNC)
	const uint32_t numCores			{ std::thread::hardware_concurrency() };
	std::vector<std::future<void>>	async_futures{};

	const uint32_t numPixelPerTask	{ numPixels / numCores };
	uint32_t numUnassignedPixels	{ numPixels % numCores };
	uint32_t currPixelIndex			{0};
		
	for (uint32_t coreId{}; coreId < numCores; ++coreId)
	{
		uint32_t taskSize = numPixelPerTask;
		if (numUnassignedPixels > 0)
		{
			++taskSize;
			--numUnassignedPixels;
		}
		async_futures.push_back(std::async(std::launch::async, [=, this]
			{
				const uint32_t pixelIndexEnd = currPixelIndex + taskSize;
				for (uint32_t pixelIndex{ currPixelIndex }; pixelIndex < pixelIndexEnd; ++pixelIndex)
				{
					RenderPixel(pScene, pixelIndex, FOV, aspectRatio, camera, lights, materials);
				}
			})
		);
		currPixelIndex += taskSize;
				
	}
	for (const std::future<void>& f : async_futures)
	{
		f.wait();
	}

#elif defined(PARALLEL_FOR)
	//Parellel-For Execution
	concurrency::parallel_for(static_cast<uint32_t>(0), numPixels, [=, this](int i)
		{
			RenderPixel(pScene, i, FOV, aspectRatio, camera, lights, materials);
		});


#else
		//Synchronos execution
		for (uint32_t i{}; i < numPixels; ++i)
		{
			RenderPixel(pScene, i, FOV, aspectRatio, camera, lights, materials);
		}
#endif

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void Renderer::CycleLightingMode()
{
	switch (m_CurrentLightingMode)
	{
	case LightingMode::ObservedArea:
		std::cout << "CYCLE MODE: Radience" << "\n";
		m_CurrentLightingMode = LightingMode::Radiance;
		break;
	case LightingMode::Radiance:
		std::cout << "CYCLE MODE: BRDF" << "\n";
		m_CurrentLightingMode = LightingMode::BRDF;
		break;
	case LightingMode::BRDF:
		std::cout << "CYCLE MODE: Combined" << "\n";
		m_CurrentLightingMode = LightingMode::Combined;
		break;
	case LightingMode::Combined:
		std::cout << "CYCLE MODE: Observed" << "\n";
		m_CurrentLightingMode = LightingMode::ObservedArea;
		break;
	}
}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Camera& camera, const std::vector<Light>& lights, const std::vector<Material*>& materials) const
{
	float offset{ 0.0001f };

	const int px = pixelIndex % m_Width;
	const int py = pixelIndex / m_Width;

	float rx = px + 0.5f;
	float ry = py + 0.5f;

	////calculating camera posX and posY based on the given formula
	float cx = (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov;
	float cy = (1 - (2 * (ry / float(m_Height)))) * fov;

	//creating a look vector for the camera
	Vector3 rayDirection{ cx, cy, 1.0f };

	rayDirection = camera.cameraToWorld.TransformVector(rayDirection);
	rayDirection.Normalize();

	Ray viewRay(camera.origin, rayDirection);

	ColorRGB finalColor{};
	HitRecord closestHit{};
	ColorRGB eRGB{};
	ColorRGB BRDF{};
	pScene->GetClosestHit(viewRay, closestHit);

	if (closestHit.didHit)
	{
		for (const Light& light : lights)
		{
			Vector3 lightDirection{ LightUtils::GetDirectionToLight(light, closestHit.origin) };
			float lightDistance = lightDirection.Normalize();

			float lambertCosine{ Vector3::Dot(closestHit.normal, lightDirection) }; //aka ObservedArea
			if ((lambertCosine < 0))
				continue;  // Skip if observedArea is negative

			Ray lightRay{ closestHit.origin + (closestHit.normal * offset), lightDirection, offset, lightDistance };

			if (m_ShadowsEnabled)
			{
				if (pScene->DoesHit(lightRay))
					continue;
			}

			eRGB = LightUtils::GetRadiance(light, closestHit.origin);
			BRDF = materials[closestHit.materialIndex]->Shade(closestHit, lightDirection, -rayDirection);

			

			switch (m_CurrentLightingMode)
			{
			case dae::Renderer::LightingMode::ObservedArea:
				finalColor += ColorRGB{ lambertCosine, lambertCosine, lambertCosine };
				break;
			case dae::Renderer::LightingMode::Radiance:
				finalColor += eRGB;
				break;
			case dae::Renderer::LightingMode::BRDF:
				finalColor += BRDF;
				break;
			case dae::Renderer::LightingMode::Combined:
				finalColor += eRGB * lambertCosine * BRDF;
				break;
			}
		}

	}
	finalColor.MaxToOne();
	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));

}