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
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();


	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			//asPect ration calculation and variablesa
			float aspectRatio{ static_cast<float>(m_Width) / static_cast<float>(m_Height) };
			float FOV{ tanf(camera.fovAngle * TO_RADIANS / 2) };
			//calculating camera posX and posY based on the given formula
			float cameraX{ (2 * ((px + 0.5f) / static_cast<float>(m_Width)) - 1) * aspectRatio * FOV };
			float cameraY{ (1 - 2 * ((py + 0.5f) / static_cast<float>(m_Height))) * FOV };


			//creating a look vector for the camera
			Vector3 rayDirection{ cameraX, cameraY, 1.0f};
			const Vector3 lightRay{ cameraX, cameraY + 100.f, 1.0f };


			//creating a camera matrix
			const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };
			rayDirection = cameraToWorld.TransformVector(rayDirection);
			rayDirection.Normalize();

			Ray viewRay(camera.origin, rayDirection);


			ColorRGB finalColor{};
			HitRecord closesHit{};
			pScene->GetClosestHit(viewRay, closesHit);
	
			if (closesHit.didHit)
			{
				finalColor = materials[closesHit.materialIndex]->Shade(closesHit, rayDirection, camera.forward);
			}

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