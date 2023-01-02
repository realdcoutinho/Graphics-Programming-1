#include "pch.h"
#include "Effect.h"
#include <assert.h>
//namespace dae
//{
//	Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile) :
//		m_pDevice(pDevice),
//		m_AssetFile(assetFile)
//	{
//		m_pEffect = LoadEffect(m_pDevice, m_AssetFile);
//		m_pTechnique = m_pEffect->GetTechniqueByName("TechniqueLinear");
//		if (!m_pTechnique->IsValid())
//		{
//			std::wcout << L"Technique is not valid" << '\n';
//		}
//
//		//transfering the matrix from cpu to gpu to use in the shader aka linking
//		m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
//		if (!m_pMatWorldViewProjVariable->IsValid())
//		{
//			std::wcout << L"m_pMatWorldViewProjVariable not valid" << '\n';
//		}
//
//		m_pMatWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
//		if (!m_pMatWorldMatrixVariable->IsValid())
//		{
//			std::wcout << L"m_pMatWorldMatrixVariable not valid" << '\n';
//		}
//
//		m_pMatInverseViewMatrixVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
//		if (!m_pMatInverseViewMatrixVariable->IsValid())
//		{
//			std::wcout << L"m_pMatInverseViewMatrixVariable not valid" << '\n';
//		}
//
//
//
//
//		m_pEffectSamplerVariable = m_pEffect->GetVariableByName("MeshTextureSampler")->AsSampler();
//		if (!m_pEffectSamplerVariable->IsValid())
//		{
//			std::wcout << L"m_pEffectSamplerVariable not valid" << '\n';
//		}
//
//		SetShaderResources();
//
//
//		//D3D11_SAMPLER_DESC samplerDesc{};
//		//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
//
//
//		//create sampler bellowe ->
//		
//		//D3D11_SAMPLER_DESC samplerDesc{};
//		//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
//
//		//D3DX11_PASS_DESC passDes{};
//		//m_pTechnique->GetPassByIndex(0)->GetDesc(&passDes);
//
//		//const HRESULT result = pDevice->CreateInputLayout(
//		//	vertexDesc,
//		//	numElements,
//		//	passDes.pIAInputSignature,
//		//	passDes.IAInputSignatureSize,
//		//	&m_pInputLayout);
//
//
//
//
//
//
//		//Create Vertex Layout
//		static constexpr uint32_t numElements{ 5 };
//		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};
//
//		vertexDesc[0].SemanticName = "POSITION";
//		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
//		vertexDesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //0
//		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
//
//		vertexDesc[1].SemanticName = "COLOR";
//		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
//		vertexDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //12
//		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
//
//		vertexDesc[2].SemanticName = "TEXCOORD";
//		vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
//		vertexDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //24
//		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
//
//		vertexDesc[3].SemanticName = "NORMAL";
//		vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
//		vertexDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //32
//		vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
//
//		vertexDesc[4].SemanticName = "TANGENT";
//		vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
//		vertexDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //40
//		vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
//
//		//Create Input Layout
//		D3DX11_PASS_DESC passDes{};
//		m_pTechnique->GetPassByIndex(0)->GetDesc(&passDes);
//
//		const HRESULT result = pDevice->CreateInputLayout(
//			vertexDesc,
//			numElements,
//			passDes.pIAInputSignature,
//			passDes.IAInputSignatureSize,
//			&m_pInputLayout);
//
//		if (FAILED(result))
//			assert(false); //or return
//	}
//
//	void Effect::SetShaderResources()
//	{
//
//		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
//		if (!m_pDiffuseMapVariable->IsValid())
//		{
//			std::wcout << L"m_pDiffuseMapVariable not valid" << '\n';
//		}
//
//		m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
//		if (!m_pNormalMapVariable->IsValid())
//		{
//			std::wcout << L"m_pNormalMapVariable not valid" << '\n';
//		}
//
//		m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
//		if (!m_pSpecularMapVariable->IsValid())
//		{
//			std::wcout << L"m_pSpecularMapVariable not valid" << '\n';
//		}
//
//		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
//		if (!m_pGlossinessMapVariable->IsValid())
//		{
//			std::wcout << L"m_pGlossinessMapVariable not valid" << '\n';
//		}
//
//	}
//
//	Effect::~Effect()
//	{
//		if (m_pDevice)
//			m_pDevice->Release();
//		if (m_pEffect)
//			m_pEffect->Release();
//		//if (m_pMatWorldViewProjVariable)
//		//	m_pMatWorldViewProjVariable->Release();
//		//if (m_pDiffuseMapVariable)
//		//	m_pDiffuseMapVariable->Release();
//	}
//
//	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
//	{
//		HRESULT result;
//		ID3D10Blob* pErrorBlob{ nullptr };
//		ID3DX11Effect* pEffect;
//
//		DWORD shaderFlags = 0;
//#if defined( DEBUG ) || defined (_DEBUG )
//		shaderFlags |= D3DCOMPILE_DEBUG;
//		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
//#endif
//		result = D3DX11CompileEffectFromFile(assetFile.c_str(),
//			nullptr,
//			nullptr,
//			shaderFlags,
//			0,
//			pDevice,
//			&pEffect,
//			&pErrorBlob);
//
//		if (FAILED(result))
//		{
//			if (pErrorBlob != nullptr)
//			{
//				const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());
//
//				std::wstringstream ss;
//				for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
//				{
//					ss << pErrors[i];
//				}
//
//				OutputDebugStringW(ss.str().c_str());
//				pErrorBlob->Release();
//				pErrorBlob = nullptr;
//
//				std::wcout << ss.str() << std::endl;
//			}
//			else
//			{
//				std::wstringstream ss;
//				ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
//				std::wcout << ss.str() << std::endl;
//				return nullptr;
//			}
//		}
//
//		return pEffect;
//	}
//
//	ID3DX11Effect* Effect::GetEffectPointer() const
//	{
//		return m_pEffect;
//	}
//
//	ID3DX11EffectTechnique* Effect::GetTechniquePointer() const
//	{
//		return m_pTechnique;
//	}
//
//	ID3D11InputLayout* Effect::GetInputLayout() const
//	{
//		return m_pInputLayout;
//	}
//
//	ID3DX11EffectMatrixVariable* Effect::GetMatWorldViewProjVariable() const
//	{
//		return m_pMatWorldViewProjVariable;
//	}
//
//	ID3DX11EffectMatrixVariable* Effect::GetMatWorldMatrixVariable() const
//	{
//		return m_pMatWorldMatrixVariable;
//	}
//
//	ID3DX11EffectMatrixVariable* Effect::GetMatInvViewMatrixVariable() const
//	{
//		return m_pMatInverseViewMatrixVariable;
//	}
//
//
//	void Effect::ToggleSamplerTechnique()
//	{
//		const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
//		if (pKeyboardState[SDL_SCANCODE_F2])
//		{
//			switch (m_SamplerTechnique) {
//			case samplerTechnique::Point:
//				m_SamplerTechnique = samplerTechnique::Linear;
//				m_pTechnique->Release();
//				m_pTechnique = m_pEffect->GetTechniqueByName("TechniquePoint");
//				if (!m_pTechnique->IsValid())
//				{
//					std::wcout << L"Technique is not valid" << '\n';
//				}
//				std::wcout << L"Technique is Point" << '\n';
//				break;
//			case samplerTechnique::Linear:
//				m_SamplerTechnique = samplerTechnique::Anisotropic;
//				m_pTechnique->Release();
//				m_pTechnique = m_pEffect->GetTechniqueByName("TechniqueLinear");
//				if (!m_pTechnique->IsValid())
//				{
//					std::wcout << L"Technique is not valid" << '\n';
//				}
//				std::wcout << "Technique is Linear" << '\n';
//				break;
//			case samplerTechnique::Anisotropic:
//				m_SamplerTechnique = samplerTechnique::Point;
//				m_pTechnique->Release();
//				m_pTechnique = m_pEffect->GetTechniqueByName("TechniqueAnsotropic");
//				if (!m_pTechnique->IsValid())
//				{
//					std::wcout << L"Technique is not valid" << '\n';
//				}
//				std::wcout << L"Technique is Ansotropic" << '\n';
//				break;
//			}
//		}
//	}
//
//
//
//
//
//#pragma region SetTextures
//	void Effect::SetDiffuseMap(Texture* pTextureMap)
//	{
//		if (m_pDiffuseMapVariable)
//		{
//			m_pDiffuseMapVariable->SetResource(pTextureMap->GetShaderResourceView());
//		}
//		else
//		{
//			std::wcout << L"Texture Diffuse not set as a Shader Resource View" << '\n';
//		}
//	}
//
//	void Effect::SetNormalMap(Texture* pTextureMap)
//	{
//		if (m_pNormalMapVariable)
//		{
//			m_pNormalMapVariable->SetResource(pTextureMap->GetShaderResourceView());
//		}
//		else
//		{
//			std::wcout << L"Texture Normal not set as a Shader Resource View" << '\n';
//		}
//	}
//
//	void Effect::SetSpecularMap(Texture* pTextureMap)
//	{
//		if (m_pSpecularMapVariable)
//		{
//			m_pSpecularMapVariable->SetResource(pTextureMap->GetShaderResourceView());
//		}
//		else
//		{
//			std::wcout << L"Texture Specular not set as a Shader Resource View" << '\n';
//		}
//	}
//
//	void Effect::SetGlossinessMap(Texture* pTextureMap)
//	{
//		if (m_pGlossinessMapVariable)
//		{
//			m_pGlossinessMapVariable->SetResource(pTextureMap->GetShaderResourceView());
//		}
//		else
//		{
//			std::wcout << L"Texture Glossiness not set as a Shader Resource View" << '\n';
//		}
//	}
//#pragma endregion SetTextures
//
//}
//
