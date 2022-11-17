#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_events.h>


#include "Math.h"
#include "Timer.h"
#include <iostream>
#include "Renderer.h"



namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{
		}

		Vector3 origin{};
		float fovAngle{ 45.f };
		const float cameraVelocity{ 5.f };

		//mine
		float fov{ tanf(fovAngle * TO_RADIANS / 2) };

	
		Vector3 worldUp{ Vector3::UnitY };
		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{ 0.f };
		float totalYaw{ 0.f };

		Matrix cameraToWorld{};

		Matrix CalculateCameraToWorld()
		{
			right = Vector3::Cross(worldUp, forward);
			right.Normalize();
			up = Vector3::Cross(forward, right);
			up.Normalize();

			const Vector4 rightVector4{ right, 0 },
					upVector4{ up, 0 },
					forwardVector4{ forward, 0 },
					originVector4{ origin, 1 };

			Matrix ONB{ rightVector4, upVector4, forwardVector4, originVector4 };
			cameraToWorld = ONB;
			return ONB;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			KeyboardInput(deltaTime);

			//Mouse Input
			MouseInput(deltaTime);
		}

		void KeyboardInput(float deltaTime)
		{
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_A])
			{
				origin += forward * cameraVelocity * deltaTime * pKeyboardState[SDL_SCANCODE_W];
				origin += forward * cameraVelocity * -deltaTime * pKeyboardState[SDL_SCANCODE_S];
				origin += right * cameraVelocity * deltaTime * pKeyboardState[SDL_SCANCODE_D];
				origin += right * cameraVelocity * -deltaTime * pKeyboardState[SDL_SCANCODE_A];
				//fovAngle += 1 * pKeyboardState[SDL_SCANCODE_UP];
				//fovAngle += 1 * pKeyboardState[SDL_SCANCODE_DOWN];
				//m_FOV = tanf(fovAngle * TO_RADIANS / 2) * pKeyboardState[SDL_SCANCODE_UP];
				//m_FOV = tanf(fovAngle * TO_RADIANS / 2) * pKeyboardState[SDL_SCANCODE_DOWN];
			}

		}

		void MouseInput(float deltaTime)
		{
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			const float leftMouse{ static_cast<float>(mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) };
			const float rightMouse{ static_cast<float>(mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) };

			if (mouseState)
			{
				if (leftMouse && rightMouse)
				{
					origin += Vector3::UnitY * -deltaTime * static_cast<float>(mouseY) * cameraVelocity;
					return;
				}
				origin += forward * -deltaTime * static_cast<float>(mouseY) * cameraVelocity * leftMouse;

				totalYaw += deltaTime * static_cast<float>(mouseX) * leftMouse;
				totalYaw += deltaTime * static_cast<float>(mouseX) * rightMouse;

				totalPitch += -deltaTime * static_cast<float>(mouseY) * rightMouse;

				const Matrix finalRotation{ Matrix::CreateRotation(totalPitch, totalYaw, 0) };
				forward = finalRotation.TransformVector(Vector3::UnitZ);
			}
		}

	};
}