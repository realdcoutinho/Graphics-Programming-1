#pragma once
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"

namespace dae
{
	class Texture
	{
	public:
		~Texture();

		static Texture* LoadFromFile(ID3D11Device* pDevice, const std::string& path);
		ID3D11ShaderResourceView* GetShaderResourceView() const;

	private:
		Texture();
		ID3D11Texture2D* m_pResource{}; //resouce
		ID3D11ShaderResourceView* m_pShaderResourceView; //shader resouce view
	};
}