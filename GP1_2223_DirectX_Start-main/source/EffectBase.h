#pragma once
#include "pch.h"
#include "Texture.h"


namespace dae
{
	class EffectBase 
	{
	public:
		EffectBase(ID3D11Device* pDevice, const std::wstring& assetFile);
		EffectBase(const EffectBase&) = delete;
		EffectBase(EffectBase&&) noexcept = delete;
		EffectBase& operator=(const EffectBase&) = delete;
		EffectBase& operator=(EffectBase&&) noexcept = delete;
		virtual ~EffectBase();

		ID3DX11Effect* GetEffectPointer() const;
		ID3DX11EffectTechnique* GetTechniquePointer() const;
		ID3D11InputLayout* GetInputLayout() const;
		ID3DX11EffectMatrixVariable* GetMatWorldViewProjVariable() const;
		ID3DX11EffectMatrixVariable* GetMatWorldMatrixVariable() const;
		ID3DX11EffectMatrixVariable* GetMatInvViewMatrixVariable() const;

		void ToggleSamplerTechnique();


	protected:

		enum class samplerTechnique
		{
			Point,
			Linear,
			Anisotropic
		};

		samplerTechnique m_SamplerTechnique = samplerTechnique::Point;
		std::wstring m_AssetFile;

		ID3D11Device* m_pDevice;
		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};
		ID3D11InputLayout* m_pInputLayout;
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable; //WORLD VIEW PROJECTION MATRIX 
		ID3DX11EffectMatrixVariable* m_pMatWorldMatrixVariable; // WORLD MATRIX
		ID3DX11EffectMatrixVariable* m_pMatInverseViewMatrixVariable; //inverse view matrix

		ID3DX11EffectSamplerVariable* m_pEffectSamplerVariable;
	};

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

}

