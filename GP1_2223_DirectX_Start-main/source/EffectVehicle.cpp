#include "pch.h"
#include "EffectVehicle.h"

namespace dae
{
	EffectVehicle::EffectVehicle(ID3D11Device* pDevice, const std::wstring& assetFile)
		: EffectBase(pDevice, assetFile)
	{
		std::wcout << L"EffectVehicle Created" << '\n';
		SetShaderResources();
	}

	EffectVehicle::~EffectVehicle()
	{
		std::wcout << L"EffectVehicle Deleted" << '\n';

		if (m_pDevice)
			m_pDevice->Release();
		if (m_pEffect)
			m_pEffect->Release();
	}

	void EffectVehicle::SetShaderResources()
	{

		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
		{
			std::wcout << L"m_pDiffuseMapVariable not valid" << '\n';
		}

		m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
		if (!m_pNormalMapVariable->IsValid())
		{
			std::wcout << L"m_pNormalMapVariable not valid" << '\n';
		}

		m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
		if (!m_pSpecularMapVariable->IsValid())
		{
			std::wcout << L"m_pSpecularMapVariable not valid" << '\n';
		}

		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
		if (!m_pGlossinessMapVariable->IsValid())
		{
			std::wcout << L"m_pGlossinessMapVariable not valid" << '\n';
		}

	}


#pragma region SetTextures
	void EffectVehicle::SetDiffuseMap(Texture* pTextureMap)
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

	void EffectVehicle::SetNormalMap(Texture* pTextureMap)
	{
		if (m_pNormalMapVariable)
		{
			m_pNormalMapVariable->SetResource(pTextureMap->GetShaderResourceView());
		}
		else
		{
			std::wcout << L"Texture Normal not set as a Shader Resource View" << '\n';
		}
	}

	void EffectVehicle::SetSpecularMap(Texture* pTextureMap)
	{
		if (m_pSpecularMapVariable)
		{
			m_pSpecularMapVariable->SetResource(pTextureMap->GetShaderResourceView());
		}
		else
		{
			std::wcout << L"Texture Specular not set as a Shader Resource View" << '\n';
		}
	}

	void EffectVehicle::SetGlossinessMap(Texture* pTextureMap)
	{
		if (m_pGlossinessMapVariable)
		{
			m_pGlossinessMapVariable->SetResource(pTextureMap->GetShaderResourceView());
		}
		else
		{
			std::wcout << L"Texture Glossiness not set as a Shader Resource View" << '\n';
		}
	}
#pragma endregion SetTextures

}

