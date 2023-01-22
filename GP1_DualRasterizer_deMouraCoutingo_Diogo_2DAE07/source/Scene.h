#pragma once
#pragma once
#include "Mesh.h"
#include "Camera.h"


namespace dae
{
	class Scene final
	{
	public:
		Scene(SDL_Window* pWindow);
		~Scene();

		Camera& GetCamera();
		Mesh* GetMesh();

	private:

		void InitializeCamera();
		void InitializeMesh();
		void PrintInitialMessage();

		Camera m_Camera;
		Mesh* m_pVehicleMesh;

		int m_Width;
		int m_Height;
		const std::string m_VehiclePath;
		float m_Fov;
		Vector3 m_Origin;

	};
}

