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

		void ToggleCullMode();
		void ToggleSamplerState();
		virtual void SetTechnique() override;




		virtual void SetDiffuseMap(Texture* pDiffuseTexture) override;
		virtual void SetNormalMap(Texture* pTextureMap) override;
		virtual void SetSpecularMap(Texture* pTextureMap) override;
		virtual void SetGlossinessMap(Texture* pTextureMap) override;

	private:
		void SetShaderResources();


		enum class CullMode
		{
			Front,
			Back,
			None
		};
		CullMode m_CullMode = CullMode::Front;

		enum class SamplerState
		{
			Point,
			Linear,
			Anisotropic
		};
		SamplerState m_SamplerState = SamplerState::Point;

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;
	};
}


