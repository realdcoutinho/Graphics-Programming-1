#include "pch.h"
#include "Render_Hardware.h"

namespace dae {

	Render_Hardware::Render_Hardware(SDL_Window* pWindow) :
		Render_Base(pWindow),
		m_IsFireOn{true},
		m_IsInitialized{false},
		m_ToggleTechnique{false}
	{
		DirectXInitialization();
		InitializeVehicleParameters();
		InitializeFire();
	}

	void Render_Hardware::DirectXInitialization()
	{
		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();

		if (result == S_OK)
		{
			m_IsInitialized = true;
			//std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			//std::cout << "DirectX initialization failed!\n";
		}
	}


	void Render_Hardware::InitializeVehicleParameters()
	{
		////vehicle
		const std::wstring shaderPath{ L"Resources/PosCol3D.fx" };
		m_pVehicleMesh->CreateBuffers(m_pDevice);
		m_pVehicleMesh->SetEffect(m_pDevice, shaderPath, "Vehicle");
		m_pVehicleMesh->SetDiffuse(m_DiffusePath);
		m_pVehicleMesh->SetNormal(m_NormalPath);
		m_pVehicleMesh->SetSpecular(m_SpecularPath);
		m_pVehicleMesh->SetGloss(m_GlossPath);
	}

	void Render_Hardware::InitializeFire()
	{
		const std::string firePath{ "Resources/fireFX.obj" };
		const std::wstring shaderPath{ L"Resources/FireShafing.fx" };
		const std::string diffusePath{ "Resources/fireFX_diffuse.png" };

		m_pFireMesh = new Mesh(firePath);
		m_pFireMesh->CreateBuffers(m_pDevice);
		m_pFireMesh->SetEffect(m_pDevice, shaderPath, "Fire");
		m_pFireMesh->SetDiffuse(diffusePath);
	}

	Render_Hardware::~Render_Hardware()
	{
		FixResourceLeaks();
		//delete m_Camera;
		delete m_pVehicleMesh;
		delete m_pFireMesh;
	}

	void Render_Hardware::Update(const Timer* pTimer)
	{
		UpdateCamera(pTimer);
		UpdateYawn(pTimer);
		m_pVehicleMesh->Update(m_Camera, m_ToggleTechnique, m_Yawn);
		m_pFireMesh->Update(m_Camera, m_ToggleTechnique, m_Yawn);
		m_ToggleTechnique = false;
	}


	void Render_Hardware::Render() const
	{
		if (!m_IsInitialized)
			return;

		//1. CLEAR RTV & DSV
		ColorRGB clearColor = ColorRGB{ 0.39f, 0.59f, 0.93f };
		if(m_IsUniformColorOn)
			clearColor = ColorRGB{ 0.1f, 0.1f, 0.1f };
		m_pDeviceContext->ClearRenderTargetView(m_pTargetRenderView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//2. SET PIPELINE + INVOKE DRAWCALLS (= RENDER)
		//...

		m_pVehicleMesh->Render(m_pDeviceContext);
		if(m_IsFireOn)
			m_pFireMesh->Render(m_pDeviceContext);
		
		//3. PRESENT BACKBUFFER (SWAP)
		m_pSwapChain->Present(0, 0);

		//4. render

	}

	HRESULT Render_Hardware::InitializeDirectX()
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

	void Render_Hardware::FixResourceLeaks()
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

	void Render_Hardware::ToggleCullMode()
	{
		m_pVehicleMesh->GetEffect()->ToggleCullMode();
	}

	void Render_Hardware::ToggleSamplerState()
	{
		m_pVehicleMesh->GetEffect()->ToggleSamplerState();
	}

	void Render_Hardware::ToggleFire()
	{
		m_IsFireOn = !m_IsFireOn;
		HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (m_IsFireOn)
		{
			SetConsoleTextAttribute(h, 2);
			std::cout << "**(HARDWARE) Fire FX On" << '\n';
		}
		if (!m_IsFireOn)
		{
			SetConsoleTextAttribute(h, 2);
			std::cout << "**(HARDWARE) Fire FX Off" << '\n';

		}
	}
}

