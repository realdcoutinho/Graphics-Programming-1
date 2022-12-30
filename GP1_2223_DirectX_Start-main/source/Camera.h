#pragma once
#include "pch.h"


namespace dae
{
	class Camera final
	{
	public:
		Camera(Vector3 origin, float fovAngle);
		~Camera();


		Camera(const Camera&) = delete;
		Camera(Camera&&) noexcept = delete;
		Camera& operator=(const Camera&) = delete;
		Camera& operator=(Camera&&) noexcept = delete;


		void Initialize(float aspectRatioRender, float fovAngle, Vector3 origin);
		void Update(float dTime);
		Matrix GetViewMatrix() const;
		Matrix GetInvViewMatrix() const;
		Matrix GetProjectionMatrix() const;
		Matrix GetWorldViewProjectionMatrix() const;
		



	private:
		void CreateViewMatrix();
		void CreateProjectionMatrix();
		void CreateWorldViewProjectionMatrix();

		void KeyboardInput(float deltaTime);
		void MouseInput(float deltaTime);

		float m_CameraVelocity{ 15.0f };
		float m_FOVAngle;
		float m_AspectRatio;
		float m_FOV;
		float m_NearPlane{ 0.1f };
		float m_FarPlane{ 100.0f };
		Vector3 m_Origin;


		Matrix m_ViewMatrix{};
		Matrix m_ProjectionMatrix{};
		Matrix m_WorldViewProjectionMatrix;

		float m_TotalPitch{};
		float m_TotalYaw{};

		Vector3 worldUp{ Vector3::UnitY };
		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };
	};
}

