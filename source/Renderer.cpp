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

#define ASYNC
//#define PARALLEL_FOR

#include <future> //ASYNC
#include <ppl.h>


Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

//void Renderer::Render(Scene* pScene) const
//{
//	Camera& camera = pScene->GetCamera();
//	auto& materials = pScene->GetMaterials();
//	auto& lights = pScene->GetLights();
//
//	std::cout << camera.forward.x << camera.forward.y, camera.forward.z;
//
//
//	for (int px{}; px < m_Width; ++px)
//	{
//		for (int py{}; py < m_Height; ++py)
//		{
//#pragma region original render
//
//			float gradient = px / static_cast<float>(m_Width);
//			//gradient += py / static_cast<float>(m_Width);
//			//gradient /= 2.0f;
//			//ColorRGB finalColor{ gradient, gradient, gradient };
//			////Update Color in Buffer
//			//finalColor.MaxToOne();
//
//			//m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
//			//	static_cast<uint8_t>(finalColor.r * 255),
//			//	static_cast<uint8_t>(finalColor.g * 255),
//			//	static_cast<uint8_t>(finalColor.b * 255));
//
//#pragma endregion original render
//
//
//			//asPect ration calculation and variable
//			float aspectRatio{ static_cast<float>(m_Width) / static_cast<float>(m_Height) };
//
//			//calculating camera posX and posY based on the given formula
//			float cameraPosX{ (((2 * (px + 0.5f)) / static_cast<float>(m_Width)) - 1) * aspectRatio };
//			float cameraPosY{ 1 - (2 * py / static_cast<float>(m_Height)) };
//
//
//			//creating a loof vector for the camera
//			Vector3 rayDirection{ cameraPosX, cameraPosY, 1 };
//			//normalazing said vector
//			Vector3 normalizedRayDirection{ rayDirection.Normalized() };
//
//			//CameraOrigin is where the look vector begins
//			Vector3 cameraOrigin{ 0, 0, 0 };
//			Ray hitRay{ cameraOrigin, normalizedRayDirection };
//			Ray viewRay(camera.origin, normalizedRayDirection);
//
//
//#pragma region squares render
//			
//			ColorRGB finalColorSquares{ normalizedRayDirection.x, normalizedRayDirection.y, normalizedRayDirection.z };
//			finalColorSquares.MaxToOne();
//
//
//			//m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
//			//	static_cast<uint8_t>(finalColorSquares.r * 255),
//			//	static_cast<uint8_t>(finalColorSquares.g * 255),
//			//	static_cast<uint8_t>(finalColorSquares.b * 255));
//
//#pragma endregion squares render
//
//#pragma region center circle render
//
//			
//			//ColorRGB finalColorSphere{};
//			//finalColorSphere.MaxToOne();
//			//HitRecord closestHitCircle{};
//
//			//Sphere testSphere{ {0.0f, 0.0f, 100.0f}, 50.0f};
//
//			//GeometryUtils::HitTest_Sphere(testSphere, viewRay, closestHitCircle);
//
//			//if (closestHitCircle.didHit)
//			//{
//
//			//	finalColorSphere = materials[closestHitCircle.materialIndex]->Shade(closestHitCircle, normalizedRayDirection, camera.forward);
//
//			//	const float scaled_t = (closestHitCircle.t - 50.0f) / 40.f;
//			//	finalColorSphere = { scaled_t , scaled_t, scaled_t };
//
//			//}
//
//			//m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
//			//	static_cast<uint8_t>(finalColorSphere.r * 255),
//			//	static_cast<uint8_t>(finalColorSphere.g * 255),
//			//	static_cast<uint8_t>(finalColorSphere.b * 255));
//
//#pragma endregion center circle render
//
//
//#pragma region 2 circles
//
//			//ColorRGB finalColorTwoCircles{};
//			//HitRecord closesHitTwoCircles{};
//			//pScene->GetClosestHit(viewRay, closesHitTwoCircles);
//	
//
//			//if (closesHitTwoCircles.didHit)
//			//{
//			//	finalColorTwoCircles = materials[closesHitTwoCircles.materialIndex]->Shade(closesHitTwoCircles, normalizedRayDirection, camera.forward);
//			//}
//
//			//m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
//			//	static_cast<uint8_t>(finalColorTwoCircles.r * 255),
//			//	static_cast<uint8_t>(finalColorTwoCircles.g * 255),
//			//	static_cast<uint8_t>(finalColorTwoCircles.b * 255));
//
//
//			//Plane testPlane{ { -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f } };
//			//HitRecord closesHitPlane{};
//			//ColorRGB finalColorPlane{};
//
//
//			//GeometryUtils::HitTest_Plane(testPlane, viewRay, closesHitPlane);
//
//			//if (closesHitPlane.didHit)
//			//{
//
//			//	finalColorPlane = materials[closesHitPlane.materialIndex]->Shade(closesHitPlane, normalizedRayDirection, camera.forward);
//
//			//	//const float scaled_t = (closesHitPlane.t - 50.0f) / 40.f;
//			//	//finalColorPlane = { scaled_t , scaled_t, scaled_t };
//
//			//}
//
//			//m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
//			//	static_cast<uint8_t>(finalColorPlane.r * 255),
//			//	static_cast<uint8_t>(finalColorPlane.g * 255),
//			//	static_cast<uint8_t>(finalColorPlane.b * 255));
//
//#pragma endregion 2 circles
//
//#pragma region extra
//			//Ray viewRayPlane{ {0, 0, 0}, rayDirection };
//			//ColorRGB finalColorPlane{};
//
//			//HitRecord closesHitPlane{};
//			//Plane testPlane{ {0.0f, -50.0f, 0.f}, {0.f, 1.f, 0.f}, 0 };
//
//			//GeometryUtils::HitTest_Plane(testPlane, viewRayPlane, closesHitPlane);
//
//			//if (closesHitPlane.didHit)
//			//{
//			//	finalColorPlane = materials[closesHitPlane.materialIndex]->Shade();
//			//}
//
//#pragma endregion extra 
//
//			//creating a camera matrix
//			const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };
//			rayDirection = cameraToWorld.TransformVector(rayDirection);
//			normalizedRayDirection =  rayDirection.Normalized();
//			Ray viewRayWeek2(camera.origin, normalizedRayDirection);
//
//
//			ColorRGB finalColorWeek2{};
//			HitRecord closesHitWeek2{};
//			pScene->GetClosestHit(viewRayWeek2, closesHitWeek2);
//
//
//			if (closesHitWeek2.didHit)
//			{
//				finalColorWeek2 = materials[closesHitWeek2.materialIndex]->Shade(closesHitWeek2, normalizedRayDirection, camera.forward);
//			}
//
//			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
//				static_cast<uint8_t>(finalColorWeek2.r * 255),
//				static_cast<uint8_t>(finalColorWeek2.g * 255),
//				static_cast<uint8_t>(finalColorWeek2.b * 255));
//
//
//		}
//	}
//
//	//@END
//	//Update SDL Surface
//	SDL_UpdateWindowSurface(m_pWindow);
//}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	camera.CalculateCameraToWorld();

	float aspectRatio{ static_cast<float>(m_Width) / static_cast<float>(m_Height) };
	float FOV{ tanf(camera.fovAngle * TO_RADIANS / 2) };

	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const uint32_t numPixels = m_Width * m_Height;
	


	//asPect ration calculation and variablesa

	float offset{ 0.0001f };

	int lightSize{ static_cast<int>(lights.size()) };

#if defined(ASYNC)
	const uint32_t numCores = std::thread::hardware_concurrency();
	std::vector<std::future<void>> async_futures{};

	const uint32_t numPixelPerTask = numPixels / numCores;
	uint32_t numUnassignedPixels = numPixels % numCores;

	uint32_t currPixelIndex = 0;
		
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
	concurrency::parallel_for(0, numPixels, [=, this](int i)
		{
			RenderPixel(pScene, i, FOV, aspectRatio, camera, lights, materials);
		});


#else
		//Synchronos execution
		//
		for (uint32_t i{}; i < numPixels; ++i)
		{
			RenderPixel(pScene, i, FOV, aspectRatio, camera, lights, materials);
		}
#endif




	//for (int px{}; px < m_Width; ++px)
	//{
	//	for (int py{}; py < m_Height; ++py)
	//	{
	//		//calculating camera posX and posY based on the given formula
	//		float cameraX{ (2 * ((px + 0.5f) / static_cast<float>(m_Width)) - 1) * aspectRatio * FOV };
	//		float cameraY{ (1 - 2 * ((py + 0.5f) / static_cast<float>(m_Height))) * FOV };
		//
	//		//creating a look vector for the camera
	//		Vector3 rayDirection{ cameraX, cameraY, 1.0f};
//
	//		//creating a camera matrix
	//		const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };
	//		rayDirection = cameraToWorld.TransformVector(Vector3{cameraX, cameraY, 1}.Normalized());
	//		rayDirection.Normalize();
//
	//		Ray viewRay(camera.origin, rayDirection);
//
//
	//		ColorRGB finalColor{};
	//		HitRecord closestHit{};
	//		pScene->GetClosestHit(viewRay, closestHit);
	//
	//		if (closestHit.didHit)
	//		{
	//			//finalColor = materials[closestHit.materialIndex]->Shade(closestHit, rayDirection, camera.forward);
	//			for (int i{}; i < lightSize; ++i)
	//			{
	//				Vector3 lightDirection{ LightUtils::GetDirectionToLight(lights[i], closestHit.origin) };
	//				Vector3 lightDirectionNormalized = lightDirection.Normalized();
	//				Ray lightRay{ closestHit.origin + (closestHit.normal * offset), lightDirectionNormalized, 0.0, lightDirection.Magnitude() };
//
//
	//				ColorRGB eRGB{ LightUtils::GetRadiance(lights[i], closestHit.origin) };
	//				const ColorRGB BRDF{ materials[closestHit.materialIndex]->Shade(closestHit, lightDirectionNormalized, -rayDirection) };
		//
//
	//				float lambertCosine{}; //aka ObservedArea
	//				if (LightType::Point == lights[i].type)
	//					lambertCosine = Vector3::Dot(closestHit.normal, lightDirection.Normalized());
	//				if (LightType::Directional == lights[i].type)
	//					float lambertCosine{ Vector3::Dot(closestHit.normal, lights[i].direction) };
	//				if ((lambertCosine < 0))
	//					continue;  // Skip if observedArea is negative
//
	//				switch (m_CurrentLightingMode)
	//				{
	//				case dae::Renderer::LightingMode::ObservedArea:
	//					finalColor += ColorRGB{ lambertCosine, lambertCosine, lambertCosine };
	//					break;
	//				case dae::Renderer::LightingMode::Radiance:
	//					finalColor += eRGB;
	//					break;
	//				case dae::Renderer::LightingMode::BRDF:
	//					finalColor += BRDF;
	//					break;
	//				case dae::Renderer::LightingMode::Combined:
	//					finalColor += eRGB * lambertCosine * BRDF;
	//					break;
	//				}
		//
	//				if (m_ShadowsEnabled)
	//				{
	//					if (pScene->DoesHit(lightRay))
	//						finalColor *= 0.5f;
	//				}
//
	//			}
//
	//		}
	//		finalColor.MaxToOne();
	//		m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
	//			static_cast<uint8_t>(finalColor.r * 255),
	//			static_cast<uint8_t>(finalColor.g * 255),
	//			static_cast<uint8_t>(finalColor.b * 255));
//
	//	}
	//}

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

	float cx = (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov;
	float cy = (1 - (2 * (ry / float(m_Height)))) * fov;



	////calculating camera posX and posY based on the given formula
	//float cameraX{ (2 * ((px + 0.5f) / static_cast<float>(m_Width)) - 1) * aspectRatio * FOV };
	//float cameraY{ (1 - 2 * ((py + 0.5f) / static_cast<float>(m_Height))) * FOV };

	//creating a look vector for the camera
	Vector3 rayDirection{ cx, cy, 1.0f };

	//creating a camera matrix
	//const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };
	//rayDirection = camera.TransformVector(Vector3{ cx, cy, 1 }.Normalized());

	rayDirection = camera.cameraToWorld.TransformVector(rayDirection);
	rayDirection.Normalize();

	Ray viewRay(camera.origin, rayDirection);


	ColorRGB finalColor{};
	HitRecord closestHit{};
	pScene->GetClosestHit(viewRay, closestHit);

	if (closestHit.didHit)
	{

		for (const Light& light : lights)
		{
			Vector3 lightDirection{ LightUtils::GetDirectionToLight(light, closestHit.origin) };
			float lightDistance = lightDirection.Normalize();
			Ray lightRay{ closestHit.origin + (closestHit.normal * offset), lightDirection, 0.0001f, lightDistance };

			if (m_ShadowsEnabled)
			{
				if (pScene->DoesHit(lightRay))
					continue;
			}

			ColorRGB eRGB{ LightUtils::GetRadiance(light, closestHit.origin) };
			const ColorRGB BRDF{ materials[closestHit.materialIndex]->Shade(closestHit, lightDirection, -rayDirection) };


			float lambertCosine{ Vector3::Dot(closestHit.normal, lightDirection) }; //aka ObservedArea

			if ((lambertCosine < 0))
				continue;  // Skip if observedArea is negative

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