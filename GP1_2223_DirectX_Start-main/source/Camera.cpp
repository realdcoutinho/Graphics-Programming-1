#include "pch.h"
#include "Camera.h"


namespace dae 
{
	Camera::Camera(Vector3 origin, float fovAngle) :
	m_Origin{origin},
	m_FOVAngle{fovAngle}
	{
		std::cout << "Camera created" << '\n';
	}

	Camera::~Camera()
	{

	}

	void Camera::Update(float dTime)
	{
		CreateViewMatrix();
		//CreateProjectionMatrix();
		CreateWorldViewProjectionMatrix();
		
		KeyboardInput(dTime);
		MouseInput(dTime);

	}


	void Camera::Initialize(float aspectRatioRender, float fovAngle = 90.f, Vector3 origin = Vector3::Zero )
	{
		m_FOVAngle = fovAngle;
		m_FOV = tanf((m_FOVAngle * TO_RADIANS) / 2.f);
		m_Origin = origin;
		m_AspectRatio = aspectRatioRender;
		//CreateViewMatrix();
		CreateProjectionMatrix();
	}


	Matrix Camera::GetViewMatrix() const
	{
		//Matrix::CreateLookAtLH(m_Origin, forward, up);
		return m_ViewMatrix;
	}

	Matrix Camera::GetProjectionMatrix() const
	{
		//Matrix::CreatePerspectiveFovLH(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
		return m_ProjectionMatrix;
	}

	Matrix Camera::GetWorldViewProjectionMatrix() const
	{
		return m_WorldViewProjectionMatrix;
	}


	void Camera::CreateViewMatrix()
	{
		//right = Vector3::Cross(worldUp, forward);
		//right.Normalize();
		//up = Vector3::Cross(forward, right);
		//up.Normalize();
		//
		//const Vector4 rightVector4{ right, 0 },
		//	upVector4{ up, 0 },
		//	forwardVector4{ forward, 0 },
		//	originVector4{ m_Origin, 1 };
		//
		//Matrix ONB{ rightVector4, upVector4, forwardVector4, originVector4 };
		////invViewMatrix = ONB;
		//
		////Inverse(ONB) => ViewMatrix
		//m_ViewMatrix = Matrix::Inverse(ONB);

		m_ViewMatrix = Matrix::CreateLookAtLH(m_Origin, forward, up);
	}

	Matrix Camera::GetInvViewMatrix() const
	{
		Matrix invView = Matrix::Inverse(m_ViewMatrix);
		return invView;
	}


	void Camera::CreateProjectionMatrix()
	{
		//const Vector4 line1{ (1 / (m_AspectRatio * m_FOV)), 0, 0, 0 };
		//const Vector4 line2{ 0, (1 / m_FOV), 0, 0 };
		//const Vector4 line3{ 0, 0, (farPlane / (farPlane - nearPlane)), 1 };
		//const Vector4 line4{ 0, 0, -1 * ((farPlane * nearPlane) / (farPlane - nearPlane)), 0 };
		//const Matrix ProjectionMatrix{ line1, line2, line3, line4 };
		//m_ProjectionMatrix = ProjectionMatrix;

		m_ProjectionMatrix = Matrix::CreatePerspectiveFovLH(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
	}

	void Camera::CreateWorldViewProjectionMatrix()
	{
		m_WorldViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
	}

	void Camera::KeyboardInput(float deltaTime)
	{
		const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

		if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_A])
		{
			m_Origin += forward * m_CameraVelocity * deltaTime * pKeyboardState[SDL_SCANCODE_W];
			m_Origin += forward * m_CameraVelocity * -deltaTime * pKeyboardState[SDL_SCANCODE_S];
			m_Origin += right * m_CameraVelocity * deltaTime * pKeyboardState[SDL_SCANCODE_D];
			m_Origin += right * m_CameraVelocity * -deltaTime * pKeyboardState[SDL_SCANCODE_A];
		}
	}

	void Camera::MouseInput(float deltaTime)
	{
		int mouseX{}, mouseY{};
		const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

		const float leftMouse{ static_cast<float>(mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) };
		const float rightMouse{ static_cast<float>(mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) };

		if (mouseState)
		{
			if (leftMouse && rightMouse)
			{
				m_Origin += Vector3::UnitY * -deltaTime * static_cast<float>(mouseY) * m_CameraVelocity;
				return;
			}
			m_Origin += forward * -deltaTime * static_cast<float>(mouseY) * m_CameraVelocity * leftMouse;

			m_TotalYaw += deltaTime * static_cast<float>(mouseX) * leftMouse;
			m_TotalYaw += deltaTime * static_cast<float>(mouseX) * rightMouse;
			m_TotalPitch += -deltaTime * static_cast<float>(mouseY) * rightMouse;

			const Matrix finalRotation{ Matrix::CreateRotation(m_TotalPitch, m_TotalYaw, 0) };
			forward = finalRotation.TransformVector(Vector3::UnitZ);
		}
	}


}
