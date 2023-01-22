//#pragma once
#include "Mesh.h"
#include "Render_Base.h"


struct SDL_Window;
struct SDL_Surface;
class Effect;

namespace dae
{
	class Render_Hardware : public Render_Base
	{
	public:
		Render_Hardware(SDL_Window* pWindow);
		~Render_Hardware();

		Render_Hardware(const Render_Hardware&) = delete;
		Render_Hardware(Render_Hardware&&) noexcept = delete;
		Render_Hardware& operator=(const Render_Hardware&) = delete;
		Render_Hardware& operator=(Render_Hardware&&) noexcept = delete;

		virtual void Update(const Timer* pTimer) override;
		virtual void Render() const override;
		void ToggleTechnique() { m_ToggleTechnique = !m_ToggleTechnique; };
		
		//void ToggleFire() { m_IsFireOn = !m_IsFireOn; };
		void ToggleFire();

		void ToggleCullMode();
		void ToggleSamplerState();

	private:
		void InitializeVehicleParameters();
		void InitializeFire();
		void DirectXInitialization();

		 
		bool m_ToggleTechnique;
		bool m_IsFireOn;
		bool m_IsInitialized;

		Matrix m_ViewMatrix;
		Matrix m_ProjectionMatrix;
		Matrix m_WorldViewMatrix;
		 
		Mesh* m_pFireMesh;

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

		void FixResourceLeaks();
		 

	};
}
