#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };
		//float fov{};
		const float cameraVelocity{ 5.f };

		Vector3 worldUp{ Vector3::UnitY };
		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f})
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;
		}

		void CalculateViewMatrix()
		{
			//TODO W1
			//ONB => invViewMatrix
			right = Vector3::Cross(worldUp, forward);
			right.Normalize();
			up = Vector3::Cross(forward, right);
			up.Normalize();

			const Vector4 rightVector4{ right, 0 },
							upVector4{ up, 0 },
							forwardVector4{ forward, 0 },
							originVector4{ origin, 1 };

			Matrix ONB{ rightVector4, upVector4, forwardVector4, originVector4 };
			invViewMatrix = ONB;
			
			//Inverse(ONB) => ViewMatrix
			viewMatrix = Matrix::Inverse(invViewMatrix);

			//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
		/*	viewMatrix = Matrix::CreateLookAtLH(origin, forward, up);*/


			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
		}

		void CalculateProjectionMatrix()
		{
			//TODO W2

			//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Camera Update Logic
			//...
			KeyboardInput(deltaTime);
			MouseInput(deltaTime);


			//Update Matrices
			CalculateViewMatrix();
			CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
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
