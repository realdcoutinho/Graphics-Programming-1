#include "pch.h"
#include "Renderer.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);	

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();

		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		std::vector<Vertex_PosCol> vertices
		{
			{{0.0f, 0.5f, 0.5f}, {colors::Red}},
			{{0.5f, -0.5f, 0.5f}, {colors::Blue}},
			{{-0.5f, -0.5f, 0.5f}, {colors::Green}},
		};

		std::vector<uint32_t> indices{ 0,1,2 };

		m_pMesh = new Mesh(m_pDevice, vertices, indices);
	}

	Renderer::~Renderer()
	{
		FixResourceLeaks();
	}

	void Renderer::Update(const Timer* pTimer)
	{

	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		//1. CLEAR RTV & DSV
		ColorRGB clearColor = ColorRGB{ 0.0f, 0.0f, 0.3f };
		m_pDeviceContext->ClearRenderTargetView(m_pTargetRenderView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//2. SET PIPELINE + INVOKE DRAWCALLS (= RENDER)
		//...
		m_pMesh->Render(m_pDeviceContext);
		
		//3. PRESENT BACKBUFFER (SWAP)
		m_pSwapChain->Present(0, 0);

		//4. render

	}

	HRESULT Renderer::InitializeDirectX()
	{
		//1. Create Device Device Context
		//====
		D3D_FEATURE_LEVEL feautureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &feautureLevel,
			1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

		if (FAILED(result))
			return result;
		
		//Create DXGI Factory
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;
		//return S_FALSE;

		//2. Create SwapChain
		//====
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = 10;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		//Get the handle (HWND) from the SLD Backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_VERSION(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		
		//Create SpawChain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
			return result;

		//3. Create DepthSencil (DS) & dePTHsENTILvIEW (DSV)
		//Resource
		D3D11_TEXTURE2D_DESC depthStenciLDesc{};
		depthStenciLDesc.Width = m_Width;
		depthStenciLDesc.Height = m_Height;
		depthStenciLDesc.MipLevels = 1;
		depthStenciLDesc.ArraySize = 1;
		depthStenciLDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStenciLDesc.SampleDesc.Count = 1;
		depthStenciLDesc.SampleDesc.Quality = 0;
		depthStenciLDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStenciLDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStenciLDesc.CPUAccessFlags = 0;
		depthStenciLDesc.MiscFlags = 0;

		//View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDes{};
		depthStencilViewDes.Format = depthStenciLDesc.Format;
		depthStencilViewDes.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDes.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStenciLDesc, nullptr, &m_pDepthStenciLBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStenciLBuffer, &depthStencilViewDes, &m_pDepthStencilView);
		if (FAILED(result))
			return result;

		//4. Create RenderTarget (RT) & RenderTargetView (RTV)
		//====

		//Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		//View 
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pTargetRenderView);
		if (FAILED(result))
			return result;

		//5. Bind RTV & DSV to Output Merger Stage
		//===== 
		m_pDeviceContext->OMSetRenderTargets(1, &m_pTargetRenderView, m_pDepthStencilView);
	

		//6. Set Viewport
		//====
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		if (FAILED(result))
			return result;

		return result;
	}

	void Renderer::FixResourceLeaks()
	{
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		if (m_pDevice)
			m_pDevice->Release();
		if (m_pSwapChain)
			m_pSwapChain->Release();
		if (m_pDepthStenciLBuffer)
			m_pDepthStenciLBuffer->Release();
		if (m_pDepthStencilView)
			m_pDepthStencilView->Release();
		if (m_pRenderTargetBuffer)
			m_pRenderTargetBuffer->Release();
		if (m_pTargetRenderView)
			m_pTargetRenderView->Release();
	}



	//void Renderer::Render_W1() const
	//{

	//	////1. Set Primative Topology
	//	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	//2. Set Input Layout
	//	m_pDeviceContext->IASetInputLayout(m_pEffect)

	//}
}
