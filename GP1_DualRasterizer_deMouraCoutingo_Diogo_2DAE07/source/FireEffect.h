#pragma once
#include "EffectBase.h"

namespace dae
{
	class FireEffect final : public EffectBase
	{
	public:
		FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile); //assetFile == path that determines which effect to load
		~FireEffect();

		FireEffect(const FireEffect&) = delete;
		FireEffect(FireEffect&&) noexcept = delete;
		FireEffect& operator=(const FireEffect&) = delete;
		FireEffect& operator=(FireEffect&&) noexcept = delete;


		virtual void SetDiffuseMap(Texture* pDiffuseTexture) override;
		virtual void SetNormalMap(Texture* pTextureMap) override;
		virtual void SetSpecularMap(Texture* pTextureMap) override;
		virtual void SetGlossinessMap(Texture* pTextureMap) override;

		virtual void SetTechnique() override;
		virtual void ToggleCullMode() override;
		virtual void ToggleSamplerState() override;



	private:
		void SetShaderResources();

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	};
}
