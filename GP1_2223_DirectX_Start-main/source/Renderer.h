#pragma once
#include "Mesh.h"

struct SDL_Window;
struct SDL_Surface;
class Effect;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		void ToggleTextures() const;

		//void Render_W1() const;

	private:
		void InitializeVehicle();
		void InitializeFire();
		
		SDL_Window* m_pWindow{};


		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };
		Matrix m_ViewMatrix;
		Matrix m_ProjectionMatrix;
		Matrix m_WorldViewMatrix;
		 

		Mesh* m_pVehicleMesh;
		Mesh* m_pFireMesh;
		Camera* m_Camera;

		//DIRECTX
		HRESULT InitializeDirectX();
		//...
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStenciLBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pTargetRenderView;

		////?????
		//ID3D11Buffer* m_pVertexBuffer;
		//ID3D11Buffer* m_pIndexBuffer;

		void FixResourceLeaks();
		 

	};
}
