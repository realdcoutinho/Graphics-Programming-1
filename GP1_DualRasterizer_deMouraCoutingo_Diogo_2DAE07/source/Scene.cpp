#include "pch.h"
#include "Scene.h"
#include <windows.h>

using namespace dae;

Scene::Scene(SDL_Window* pWindow) :
	m_VehiclePath { "Resources/Vehicle.obj" },
	m_Fov{ 45.0f },
	m_Origin{ 0.0f, 0.0f, 0.0f }
{
	//Initialize "framework"
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	InitializeCamera();
	InitializeMesh();
	PrintInitialMessage();
}

Scene::~Scene()
{
	delete m_pVehicleMesh;
}

void Scene::InitializeCamera()
{
	float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	m_Camera.Initialize(aspectRatio, m_Fov, m_Origin);
}

void Scene::InitializeMesh()
{
	m_pVehicleMesh = new Mesh(m_VehiclePath);
}

void Scene::PrintInitialMessage()
{
	HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(h, 6);
	std::cout << "[Key Bindings - SHARED]" << '\n';
	std::cout << "	[F1] Toggle Rasterizer Mode(HARDWARE / SOFTWARE)" << '\n';
	std::cout << "	[F2]  Toggle Vehicle Rotation(ON / OFF)" << '\n';
	std::cout << "	[F9]  Cycle CullMode(BACK / FRONT / NONE) " << '\n';
	std::cout << "	[F10] Toggle Uniform ClearColor(ON / OFF)" << '\n';
	std::cout << "	[F11] Toggle Print FPS(ON / OFF)" << '\n';

	SetConsoleTextAttribute(h, 2);
	std::cout << "[Key Bindings - HARDWARE]" << '\n';
	std::cout << "	[F3] Toggle FireFX(ON / OFF)" << '\n';
	std::cout << "	[F4] Cycle Sampler State(POINT / LINEAR / ANISOTROPIC)" << '\n';

	SetConsoleTextAttribute(h, 5);
	std::cout << "[Key Bindings - SOFTWARE]" << '\n';
	std::cout << "	[F5] Cycle Shading Mode(COMBINED / OBSERVED_AREA / DIFFUSE / SPECULAR)" << '\n';
	std::cout << "	[F6] Toggle NormalMap(ON / OFF)" << '\n';
	std::cout << "	[F7] Toggle DepthBuffer Visualization(ON / OFF)" << '\n';
	std::cout << "	[F8] Toggle BoundingBox Visualization(ON / OFF)" << '\n';

	SetConsoleTextAttribute(h, 1);
	std::cout << "[EXTRAS]" << '\n';
	std::cout << "   -> MULTITHREADING" << '\n';
	std::cout << "   -> LIGHT STRUCTURES (POINT AND DIRECTIONAL)" << '\n';

	SetConsoleTextAttribute(h, 11);
	std::cout << "[DISCLAIMER]" << '\n';
	std::cout << "   -Mouse input functions only individually, not simultaneously on both modes" << '\n';
	std::cout << "   -It is currently running in the sofwtare mode" << '\n';
	std::cout << "   -To switch, check the updates in the main.cpp" << '\n';
	std::cout << "   -Uncomment two lines of code and comment the current ones (***)" << '\n';
	std::cout << '\n';


	SetConsoleTextAttribute(h, 15);
}

Camera& Scene::GetCamera()
{
	return m_Camera;
}

Mesh* Scene::GetMesh()
{
	return m_pVehicleMesh;
}