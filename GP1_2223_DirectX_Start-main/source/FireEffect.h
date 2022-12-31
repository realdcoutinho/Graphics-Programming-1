#pragma once
#include "EffectBase.h"

namespace dae
{
	class FireEffect : public EffectBase
	{
	public:
		FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile); //assetFile == path that determines which effect to load
		~FireEffect();

		FireEffect(const FireEffect&) = delete;
		FireEffect(FireEffect&&) noexcept = delete;
		FireEffect& operator=(const FireEffect&) = delete;
		FireEffect& operator=(FireEffect&&) noexcept = delete;


		void SetDiffuseMap(Texture* pDiffuseTexture);

	private:
		void SetShaderResources();

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;
	};
}
