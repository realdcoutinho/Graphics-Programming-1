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

		static Texture* LoadFromFile(ID3D11Device* pDevice, const std::string& path); //hardware
		ID3D11ShaderResourceView* GetShaderResourceView() const; //hardware

		static Texture* LoadFromFile(const std::string& path); // software
		ColorRGB Sample(const Vector2& uv) const; // software


	private:
		Texture(); //hardware
		ID3D11Texture2D* m_pResource{}; //resouce hardware
		ID3D11ShaderResourceView* m_pShaderResourceView; //shader resouce view hardware


		Texture(SDL_Surface* pSurface); //software

		SDL_Surface* m_pSurface{ nullptr };  //software
		uint32_t* m_pSurfacePixels{ nullptr }; //software

	};
}