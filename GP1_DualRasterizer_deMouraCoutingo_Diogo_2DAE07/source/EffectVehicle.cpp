#include "pch.h"
#include "EffectVehicle.h"

namespace dae
{
	EffectVehicle::EffectVehicle(ID3D11Device* pDevice, const std::wstring& assetFile)
		: EffectBase(pDevice, assetFile)
	{
		//std::wcout << L"EffectVehicle Created" << '\n';
		SetShaderResources();
	}

	EffectVehicle::~EffectVehicle()
	{
		//std::wcout << L"EffectVehicle Deleted" << '\n';

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


	void EffectVehicle::ToggleCullMode()
	{
		HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(h, 6);

		switch (m_CullMode)
		{
		case CullMode::Front:
			m_CullMode = CullMode::Back;
			std::cout << "**(SHARED) CullMode = Back" << '\n';
			break;
		case CullMode::Back:
			m_CullMode = CullMode::None;
			std::cout << "**(SHARED) CullMode = None" << '\n';			
			break;
		case CullMode::None:
			m_CullMode = CullMode::Front;
			std::cout << "**(SHARED) CullMode = Front" << '\n';
			break;
		}
	}


	void EffectVehicle::ToggleSamplerState()
	{
		HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(h, 2);
		switch (m_SamplerState)
		{
		case SamplerState::Point:
			m_SamplerState = SamplerState::Linear;
	
			std::cout << "**(HARDWARE) Sampler Filter = Linear" << '\n';
			break;
		case SamplerState::Linear:
			m_SamplerState = SamplerState::Anisotropic;
			std::cout << "**(HARDWARE) Sampler Filter = Anisotropic" << '\n';
			break;
		case SamplerState::Anisotropic:
			m_SamplerState = SamplerState::Point;
			std::cout << "**(HARDWARE) Sampler Filter = Point" << '\n';
			break;
		}
	}

	void EffectVehicle::SetTechnique()
	{
		switch (m_SamplerState)
		{
		case SamplerState::Point:
			switch (m_CullMode)
			{
			case CullMode::Front:
				m_pTechnique = m_pEffect->GetTechniqueByName("TechniquePointFront");
				if (!m_pTechnique->IsValid())
				{
					std::wcout << L"Technique is not valid1" << '\n';
				}
				break;
			case CullMode::Back:
				m_pTechnique = m_pEffect->GetTechniqueByName("TechniquePointBack");
				if (!m_pTechnique->IsValid())
				{
					std::wcout << L"Technique is not valid2" << '\n';
				}
				break;
			case CullMode::None:
				m_pTechnique = m_pEffect->GetTechniqueByName("TechniquePointNone");
				if (!m_pTechnique->IsValid())
				{
					std::wcout << L"Technique is not valid3" << '\n';
				}
				break;
			}
		break;
		case SamplerState::Linear:
			switch (m_CullMode)
			{
			case CullMode::Front:
				m_pTechnique = m_pEffect->GetTechniqueByName("TechniqueLinearFront");
				if (!m_pTechnique->IsValid())
				{
					std::wcout << L"Technique is not valid4" << '\n';
				}
				break;
			case CullMode::Back:
				m_pTechnique = m_pEffect->GetTechniqueByName("TechniqueLinearBack");
				if (!m_pTechnique->IsValid())
				{
					std::wcout << L"Technique is not valid5" << '\n';
				}
				break;
			case CullMode::None:
				m_pTechnique = m_pEffect->GetTechniqueByName("TechniqueLinearNone");
				if (!m_pTechnique->IsValid())
				{
					std::wcout << L"Technique is not valid6" << '\n';
				}
				break;
			}
			break;
		case SamplerState::Anisotropic:
			switch (m_CullMode)
			{
			case CullMode::Front:
				m_pTechnique = m_pEffect->GetTechniqueByName("TechniqueAnsotropicFront");
				if (!m_pTechnique->IsValid())
				{
					std::wcout << L"Technique is not valid7" << '\n';
				}
				break;
			case CullMode::Back:
				m_pTechnique = m_pEffect->GetTechniqueByName("TechniqueAnsotropicBack");
				if (!m_pTechnique->IsValid())
				{
					std::wcout << L"Technique is not valid8" << '\n';
				}
				break;
			case CullMode::None:
				m_pTechnique = m_pEffect->GetTechniqueByName("TechniqueAnsotropicNone");
				if (!m_pTechnique->IsValid())
				{
					std::wcout << L"Technique is not valid9" << '\n';
				}
				break;
			}
			break;
		}

	}






}

