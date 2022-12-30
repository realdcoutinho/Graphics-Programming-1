#include "pch.h"
#include "Texture.h"
#include <SDL_image.h>
#include <assert.h>

namespace dae
{
	Texture::Texture()
	{
		std::wcout << L"Texture SDL loaded" << '\n';
	}

	Texture::~Texture()
	{

		if (m_pResource != nullptr)
		{
			m_pResource->Release();
		}

		if (m_pShaderResourceView != nullptr)
		{
			m_pShaderResourceView->Release();
		}

	}

	Texture* Texture::LoadFromFile(ID3D11Device* pDevice, const std::string& path)
	{
		//TODO
		//Load SDL_Surface using IMG_LOAD
		//Create & Return a new Texture Object (using SDL_Surface)

		SDL_Surface* pSurface{ IMG_Load(path.c_str()) };
		assert(pSurface != nullptr && "There was no file found");

		Texture* imageTexture = new Texture{ };

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = pSurface->w;
		desc.Height = pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

		HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &imageTexture->m_pResource);

		if (FAILED(hr))
		{
			delete imageTexture;
			std::wcout << L"No Texture Created";
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResouceViewDesc{};
		shaderResouceViewDesc.Format = format;
		shaderResouceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResouceViewDesc.Texture2D.MipLevels = 1;

		hr = pDevice->CreateShaderResourceView(imageTexture->m_pResource, &shaderResouceViewDesc, &imageTexture->m_pShaderResourceView);
		
		if (FAILED(hr))
		{
			delete imageTexture;
			std::wcout << L"No Texture Created";
		}

		SDL_FreeSurface(pSurface);
		return imageTexture;
	}

	ID3D11ShaderResourceView* Texture::GetShaderResourceView() const
	{
		return m_pShaderResourceView;
	}
}