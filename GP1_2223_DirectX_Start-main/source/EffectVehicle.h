#pragma once
#include "EffectBase.h"
namespace dae
{
	class EffectVehicle : public EffectBase
	{
	public:
		EffectVehicle(ID3D11Device* pDevice, const std::wstring& assetFile); //assetFile == path that determines which effect to load
		~EffectVehicle();

		EffectVehicle(const EffectVehicle&) = delete;
		EffectVehicle(EffectVehicle&&) noexcept = delete;
		EffectVehicle& operator=(const EffectVehicle&) = delete;
		EffectVehicle& operator=(EffectVehicle&&) noexcept = delete;


		void SetDiffuseMap(Texture* pDiffuseTexture);
		void SetNormalMap(Texture* pTextureMap);
		void SetSpecularMap(Texture* pTextureMap);
		void SetGlossinessMap(Texture* pTextureMap);

	private:
		void SetShaderResources();

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;
	};
}


