#pragma once
#include "pch.h"

class Effect final
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile); //assetFile == path that determines which effect to load
	~Effect();

	Effect(const Effect&) = delete;
	Effect(Effect&&) noexcept = delete;
	Effect& operator=(const Effect&) = delete;
	Effect& operator=(Effect&&) noexcept = delete;

	ID3DX11Effect* GetEffectPointer() const;
	ID3DX11EffectTechnique* GetTechniquePointer() const;
	ID3D11InputLayout* GetInputLayout() const;

private:
	ID3D11Device* m_pDevice{};
	const std::wstring& m_AssetFile{};


	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};
	ID3D11InputLayout* m_pInputLayout;


};
static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
