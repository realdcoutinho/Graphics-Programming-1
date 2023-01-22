#include "pch.h"
#include "Render_Base.h"

namespace dae
{
	Render_Base::Render_Base(SDL_Window* pWindow) :
		m_pWindow(pWindow),
		m_IsRotationOn		{ true },
		m_IsUniformColorOn	{false},
		m_FOV				{45.0f},
		m_Yawn				{0},
		m_RotationSpeed		{1},
		m_Origin			{0.0f, 0.0f, 0.0f},
		m_VehiclePath		{ "Resources/Vehicle.obj" },
		m_DiffusePath		{ "Resources/vehicle_diffuse.png" },
		m_NormalPath		{ "Resources/vehicle_normal.png" },
		m_SpecularPath		{ "Resources/vehicle_specular.png" },
		m_GlossPath			{ "Resources/vehicle_gloss.png" }
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize Camera
		m_AspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
		m_Camera.Initialize(m_AspectRatio, m_FOV, m_Origin);
		m_Camera.CalculateViewMatrix();

		InitializeVehicle();
	}

	Render_Base::~Render_Base()
	{
	}

	void Render_Base::UpdateCamera(const Timer * pTimer)
	{
		m_Camera.Update(pTimer);
	}

	void Render_Base::UpdateYawn(const Timer* pTimer)
	{
		if (m_IsRotationOn)
			m_Yawn += pTimer->GetElapsed() * m_RotationSpeed;
	}

	void Render_Base::InitializeVehicle()
	{
		m_pVehicleMesh = new Mesh(m_VehiclePath);
	}

	void Render_Base::ToggleRotation() 
	{
		m_IsRotationOn = !m_IsRotationOn;
		HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (m_IsRotationOn)
		{
			SetConsoleTextAttribute(h, 6);
			std::cout << "**(SHARED) Vehicle Rotation On" << '\n';
		}
		if (!m_IsRotationOn)
		{
			SetConsoleTextAttribute(h, 6);
			std::cout << "**(SHARED) Vehicle Rotation Off" << '\n';

		}
	}

	void Render_Base::ToggleUniformColor()
	{
		m_IsUniformColorOn = !m_IsUniformColorOn;
		HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (m_IsUniformColorOn)
		{
			SetConsoleTextAttribute(h, 6);
			std::cout << "**(SHARED) Uniform Color On" << '\n';
		}
		if (!m_IsUniformColorOn)
		{
			SetConsoleTextAttribute(h, 6);
			std::cout << "**(SHARED) Uniform Color Rotation Off" << '\n';

		}
	}



}

