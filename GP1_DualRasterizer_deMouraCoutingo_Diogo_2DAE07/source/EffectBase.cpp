#include "pch.h"
#include "EffectBase.h"

namespace dae
{
	EffectBase::EffectBase(ID3D11Device* pDevice, const std::wstring& assetFile) :
		m_pDevice{ pDevice },
		m_AssetFile{ assetFile }
	{
		//std::wcout << L"BaseEffect Created" << '\n';

		m_pEffect = LoadEffect(m_pDevice, m_AssetFile);
		m_pTechnique = m_pEffect->GetTechniqueByName("TechniquePointFront");
		if (!m_pTechnique->IsValid())
		{
			std::wcout << L"Technique is not valid" << '\n';
		}

		//transfering the matrix from cpu to gpu to use in the shader aka linking
		m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		if (!m_pMatWorldViewProjVariable->IsValid())
		{
			std::wcout << L"m_pMatWorldViewProjVariable not valid" << '\n';
		}

		m_pMatWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
		if (!m_pMatWorldMatrixVariable->IsValid())
		{
			std::wcout << L"m_pMatWorldMatrixVariable not valid" << '\n';
		}

		m_pMatInverseViewMatrixVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
		if (!m_pMatInverseViewMatrixVariable->IsValid())
		{
			std::wcout << L"m_pMatInverseViewMatrixVariable not valid" << '\n';
		}

		m_pEffectSamplerVariable = m_pEffect->GetVariableByName("MeshTextureSampler")->AsSampler();
		if (!m_pEffectSamplerVariable->IsValid())
		{
			std::wcout << L"m_pEffectSamplerVariable not valid" << '\n';
		}

		//D3D11_SAMPLER_DESC samplerDesc{};
		//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;


		//create sampler bellowe ->

		//D3D11_SAMPLER_DESC samplerDesc{};
		//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

		//D3DX11_PASS_DESC passDes{};
		//m_pTechnique->GetPassByIndex(0)->GetDesc(&passDes);

		//const HRESULT result = pDevice->CreateInputLayout(
		//	vertexDesc,
		//	numElements,
		//	passDes.pIAInputSignature,
		//	passDes.IAInputSignatureSize,
		//	&m_pInputLayout);






		//Create Vertex Layout
		static constexpr uint32_t numElements{ 5 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //0
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //12
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "TEXCOORD";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //24
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[3].SemanticName = "NORMAL";
		vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //32
		vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[4].SemanticName = "TANGENT";
		vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; //40
		vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		//Create Input Layout
		D3DX11_PASS_DESC passDes{};
		m_pTechnique->GetPassByIndex(0)->GetDesc(&passDes);

		const HRESULT result = pDevice->CreateInputLayout(
			vertexDesc,
			numElements,
			passDes.pIAInputSignature,
			passDes.IAInputSignatureSize,
			&m_pInputLayout);

		if (FAILED(result))
			assert(false); //or return

	}

	EffectBase::~EffectBase()
	{
		//std::wcout << L"EffectBase Deleted" << '\n';

		if (m_pDevice)
			m_pDevice->Release();
		if (m_pEffect)
			m_pEffect->Release();
		if (m_pInputLayout)
			m_pInputLayout->Release();
		//if (m_pEffectSamplerVariable)
		//	m_pEffectSamplerVariable->Release();
		//why are some resoures not released
	}

	ID3DX11Effect* EffectBase::GetEffectPointer() const
	{
		return m_pEffect;
	}

	ID3DX11EffectTechnique* EffectBase::GetTechniquePointer() const
	{
		return m_pTechnique;
	}

	ID3D11InputLayout* EffectBase::GetInputLayout() const
	{
		return m_pInputLayout;
	}

	ID3DX11EffectMatrixVariable* EffectBase::GetMatWorldViewProjVariable() const
	{
		return m_pMatWorldViewProjVariable;
	}

	ID3DX11EffectMatrixVariable* EffectBase::GetMatWorldMatrixVariable() const
	{
		return m_pMatWorldMatrixVariable;
	}

	ID3DX11EffectMatrixVariable* EffectBase::GetMatInvViewMatrixVariable() const
	{
		return m_pMatInverseViewMatrixVariable;
	}

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{ nullptr };
		ID3DX11Effect* pEffect;

		DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined (_DEBUG )
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		result = D3DX11CompileEffectFromFile(assetFile.c_str(),
			nullptr,
			nullptr,
			shaderFlags,
			0,
			pDevice,
			&pEffect,
			&pErrorBlob);

		if (FAILED(result))
		{
			if (pErrorBlob != nullptr)
			{
				const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

				std::wstringstream ss;
				for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
				{
					ss << pErrors[i];
				}

				OutputDebugStringW(ss.str().c_str());
				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << std::endl;
			}
			else
			{
				std::wstringstream ss;
				ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
				std::wcout << ss.str() << std::endl;
				return nullptr;
			}
		}

		return pEffect;
	}



}

