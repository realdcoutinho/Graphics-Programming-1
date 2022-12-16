#include "pch.h"
#include "Effect.h"
#include <assert.h>

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile) :
	m_pDevice(pDevice),
	m_AssetFile(assetFile)
{
	m_pEffect = LoadEffect(m_pDevice, m_AssetFile);
	m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pTechnique->IsValid())
	{
		std::wcout << L"Technique is not valid" << '\n';
	}

	//Effect* pEffect{ new Effect(pDevice,) };
	//m_pTechnique = pEffect->GetTechniquePointer();

	//Create Vertex Layout
	static constexpr uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};
	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

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

Effect::~Effect()
{
	if (m_pDevice)
		m_pDevice->Release();
	if (m_pEffect)
		m_pEffect->Release();
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

ID3DX11Effect* Effect::GetEffectPointer() const
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechniquePointer() const
{
	return m_pTechnique;
}

ID3D11InputLayout* Effect::GetInputLayout() const
{
	return m_pInputLayout;
}