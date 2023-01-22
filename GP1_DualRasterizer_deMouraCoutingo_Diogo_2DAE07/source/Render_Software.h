#pragma once

#include <cstdint>
#include <vector>

#include "Camera.h"
#include "DataTypes.h"
#include "Render_Base.h"

struct SDL_Window;
struct SDL_Surface;

////foward declarations
//class Texture;
//struct MeshStruct;
//struct Vertex;
//class Timer;
//class Scene;


namespace dae
{
	class Render_Software : public Render_Base
	{
	public:
		Render_Software(SDL_Window* pWindow);
		~Render_Software();

		Render_Software(const Render_Software&) = delete;
		Render_Software(Render_Software&&) noexcept = delete;
		Render_Software& operator=(const Render_Software&) = delete;
		Render_Software& operator=(Render_Software&&) noexcept = delete;

		virtual void Update(const Timer* pTimer) override;
		virtual void Render() const override;
		void Render_Vehicle() const;

		void ToggleShadingMode();
		void ToggleDepthBuffer();
		void ToggleBoundingBox();
		void ToggleCullMode();
		void ToggleNormalMap();
		
		//void ToggleNormalMap() { m_IsNormalMapOn = !m_IsNormalMapOn; };



	private:

		enum class ShadingMode
		{
			ObservedArea,
			Diffuse,
			Specular,
			Combined
		};
		ShadingMode m_ShadingMode = ShadingMode::Combined;

		enum class CullMode
		{
			Front,
			Back,
			none,
		};
		CullMode m_CullMode = CullMode::Front;

		
		Light m_Light;
		MeshStruct m_MeshVehicle;

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};


		Texture* m_pDiffuse;
		Texture* m_pNormalMap;
		Texture* m_pSpecular;
		Texture* m_pGloss;

		float* m_pDepthBufferPixels{};

		bool m_IsNormalMapOn;
		bool m_IsParallelOn;
		bool m_IsDepthBufferOn;
		bool m_IsBoundingBoxOn;
		bool m_IsTextureOn;


		void CreateBuffers(SDL_Window* pWindow);
		void InitializeTextures();
		void InitializeVehicle();
		void InitializeLights();
		void RenderTriangleW4(Vertex_Out& v0, Vertex_Out& v1, Vertex_Out& v2) const;
		void VertexTransformationFunction(std::vector<MeshStruct>& mesh) const;
		ColorRGB PixelShading(const Vertex_Out& v) const;
		bool SaveBufferToImage() const;
	};
}