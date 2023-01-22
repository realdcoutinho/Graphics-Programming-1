#include "pch.h"
#include "FireEffect.h"

namespace dae
{
	FireEffect::FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
		: EffectBase(pDevice, assetFile)
	{
		//std::wcout << L"FireEffect Created" << '\n';
		SetShaderResources();
	}

	FireEffect::~FireEffect()
	{
		//std::wcout << L"FireEffect Deleted" << '\n';
	}

	void FireEffect::SetShaderResources()
	{

		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
		{
			std::wcout << L"m_pDiffuseMapVariable not valid" << '\n';
		}
	}


#pragma region SetTextures
	void FireEffect::SetDiffuseMap(Texture* pTextureMap)
	{
		if (m_pDiffuseMapVariable)
		{
			m_pDiffuseMapVariable->SetResource(pTextureMap->GetShaderResourceView());
		}
		else
		{
			std::wcout << L"Texture Diffuse not set as a Shader Resource View" << '\n';
		}
	}

	void FireEffect::SetNormalMap(Texture* pTextureMap)
	{

	}

	void FireEffect::SetSpecularMap(Texture* pTextureMap)
	{

	}

	void FireEffect::SetGlossinessMap(Texture* pTextureMap)
	{

	}

	void FireEffect::SetTechnique()
	{

	}

	void FireEffect::ToggleCullMode()
	{

	}

	void FireEffect::ToggleSamplerState()
	{

	}


#pragma endregion SetTextures

}

