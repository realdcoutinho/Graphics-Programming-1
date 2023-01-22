#pragma once
#include "Mesh.h"
//#include "Timer.h"

struct SDL_Window;
struct SDL_Surface;
//class Timer;
namespace dae
{
	class Render_Base
	{
	public:
		Render_Base(SDL_Window* pWindow);
		virtual ~Render_Base();

		Render_Base(const Render_Base&) = delete;
		Render_Base(Render_Base&&) noexcept = delete;
		Render_Base& operator=(const Render_Base&) = delete;
		Render_Base& operator=(Render_Base&&) noexcept = delete;

		virtual void Update(const Timer* pTimer) = 0;
		void UpdateCamera(const Timer* pTimer);
		void UpdateYawn(const Timer* pTimer);
		virtual void Render() const = 0;

		//void ToggleRotation() { m_IsRotationOn = !m_IsRotationOn; };
		//void ToggleUniformColor() { m_IsUniformColorOn = !m_IsUniformColorOn; };


		void ToggleRotation();
		void ToggleUniformColor();


	protected:
		Camera m_Camera;
		Mesh* m_pVehicleMesh;

		bool m_IsRotationOn;
		bool m_IsUniformColorOn;
		const float m_FOV;
		float m_Yawn;
		const float m_RotationSpeed;
		const Vector3 m_Origin;
		

		int m_Width;
		int m_Height;
		float m_AspectRatio;

		SDL_Window* m_pWindow{};

		//comomn to both modes so information kpet ion the base render
		const std::string m_VehiclePath;
		const std::string m_DiffusePath;
		const std::string m_NormalPath;
		const std::string m_SpecularPath;
		const std::string m_GlossPath;
		void InitializeVehicle();
	};
}


