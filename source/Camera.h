#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_events.h>


#include "Math.h"
#include "Timer.h"
#include <iostream>

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
		const float cameraVelocity{ 5.f };


		Vector3 worldUp{ Vector3::UnitY };
		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			right	= Vector3::Cross(worldUp, forward) ;
			right.Normalize();
			up		= Vector3::Cross(forward, right) ;
			up.Normalize();
			
			Vector4 rightVector4{ right, 0 },
				upVector4{ up, 0 },
				forwardVector4{ forward, 0 },
				originVector4{ origin, 1 };

			Matrix ONB{ rightVector4, upVector4, forwardVector4, originVector4 };
			return ONB;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += forward * cameraVelocity * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward *cameraVelocity * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += right * cameraVelocity * deltaTime;
				
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= right * cameraVelocity * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_E])
			{
				origin += up * cameraVelocity * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_Q])
			{
				origin -= up *  cameraVelocity * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_UP])
			{
				fovAngle +=1.0f;
			}
			if (pKeyboardState[SDL_SCANCODE_DOWN])
			{
				fovAngle -= 1.0f;
			}

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
		
			if (mouseState == SDL_BUTTON_LEFT)
			{
				//while (SDL_BUTTON_LEFT)
				//{
					Matrix rotationY{ Matrix::CreateRotationY(static_cast<float>(mouseX * TO_RADIANS)) };
					Matrix rotationZ{ Matrix::CreateRotationZ(static_cast<float>(mouseY * TO_RADIANS)) };
					forward = rotationY.TransformVector(forward);
					forward.Normalize();
					forward = rotationZ.TransformVector(forward);
					forward.Normalize();
				//}

			}
		
			//SDL_Event event;
			//while (SDL_PollEvent(&event))
			//{
			//	fovAngle += 1.0f;
			//	if (event.type == SDL_MOUSEWHEEL)
			//	{
			//		std::cout << "ROLLING UP" << '\n';
			//		if (event.wheel.y > 0) // scroll up
			//		{
			//			fovAngle += 1.0f;
			//			std::cout << "ROLLING UP" << '\n';
			//			// Put code for handling "scroll up" here!
			//		}
			//		else if (event.wheel.y < 0) // scroll down
			//		{
			//			std::cout << "ROLLING DOWN" << '\n';
			//
			//			fovAngle -= 1.0f;
			//			// Put code for handling "scroll down" here!
			//		}
			//
			//		if (event.wheel.x > 0) // scroll right
			//		{
			//			// ...
			//		}
			//		else if (event.wheel.x < 0) // scroll left
			//		{
			//			// ...
			//		}
			//	}
			//	else if (event.type == SDL_MOUSEBUTTONDOWN)
			//	{
			//		// ... handle mouse clicks ...
			//	}
			//
			//	// ... handle other kinds of events ...
			//}
		}
	};
}
