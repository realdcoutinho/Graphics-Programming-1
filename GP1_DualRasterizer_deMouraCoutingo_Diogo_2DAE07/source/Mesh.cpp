#include "pch.h"
#include "Mesh.h"
#include <assert.h>




namespace dae
{
	Mesh::Mesh(const std::string& vehicle)
	{
		Utils::ParseOBJ(vehicle, m_Vertices, m_Indices);
		//std::cout << "Mesh Created" << '\n';
	}

	void Mesh::CreateBuffers(ID3D11Device* pDevice)
	{
		m_pDevice = pDevice;
		//Create vertex buffer
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(m_Vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_Vertices.data();

		HRESULT result = m_pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result))
			return;

		//Create index buffer
		m_NumIndices = static_cast<uint32_t>(m_Indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		initData.pSysMem = m_Indices.data();
		result = m_pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
		if (FAILED(result))
			return;
	}

	void Mesh::SetEffect(ID3D11Device* pDevice, const std::wstring& shader, const std::string& effect)
	{
		if (effect == "Vehicle")
		{
			m_pEffectBase = new EffectVehicle(pDevice, shader);
		}
		if (effect == "Fire")
		{
			m_pEffectBase = new FireEffect(pDevice, shader);
		}
	}


	Mesh::~Mesh()
	{
		delete m_pEffectBase;
	}

	void Mesh::Update(Camera camera, bool isToggling, float yawn)
	{
		m_pEffectBase->SetTechnique();
		Matrix rotationMatrix = Matrix::CreateRotationY(yawn);
		Matrix translationMatrix = Matrix::CreateTranslation({ 0.0f, 0.0f, 50.0f });
		Matrix TR = rotationMatrix * translationMatrix;


		//set the matrix for the PosCol3D
		Matrix worldViewProjectionMatrix = TR * camera.worldViewProjectionMatrix;
		float* arrayWorldViewMatrix = reinterpret_cast<float*>(&worldViewProjectionMatrix);
		//m_pEffect->GetMatWorldViewProjVariable()->SetMatrix(arrayWorldViewMatrix);
		m_pEffectBase->GetMatWorldViewProjVariable()->SetMatrix(arrayWorldViewMatrix);

		//set the world  matrix for PosCol3D
		Matrix worldMatrix = TR * camera.viewMatrix ;
		float* arrayWorldMatrix = reinterpret_cast<float*>(&worldMatrix);
		//m_pEffect->GetMatWorldMatrixVariable()->SetMatrix(arrayWorldMatrix);
		m_pEffectBase->GetMatWorldMatrixVariable()->SetMatrix(arrayWorldMatrix);


		//set the inverse view matrix
		Matrix inverseViewMatrix = TR * camera.invViewMatrix ;
		float* arrayInverseViewMatrix = reinterpret_cast<float*>(&inverseViewMatrix);
		//m_pEffect->GetMatInvViewMatrixVariable()->SetMatrix(arrayInverseViewMatrix);
		m_pEffectBase->GetMatInvViewMatrixVariable()->SetMatrix(arrayInverseViewMatrix);

	}

	void Mesh::Render(ID3D11DeviceContext* pDeviceContext) const
	{

		////1. Set Primative Topology
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		//2. Set Input Layout
		//pDeviceContext->IASetInputLayout(m_pEffect->GetInputLayout());
		pDeviceContext->IASetInputLayout(m_pEffectBase->GetInputLayout());

		//3. SetVertexBuffer
		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;

		ID3D11Buffer* pVertexBuffer = m_pVertexBuffer;

		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		//4. set indexbuffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//5. Draw
		//renderTriangle
		D3DX11_TECHNIQUE_DESC techDesc{};

		m_pEffectBase->GetTechniquePointer()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			//m_pEffect->GetTechniquePointer()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			m_pEffectBase->GetTechniquePointer()->GetPassByIndex(p)->Apply(0, pDeviceContext);

			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}

	}

	void Mesh::SetDiffuse(const std::string& diffuse)
	{
		Texture* diffuseMap = Texture::LoadFromFile(m_pDevice, diffuse);
		m_pEffectBase->SetDiffuseMap(diffuseMap);
		//if (m_pEffectBase)
		//{
		//	m_pEffectBase->SetDiffuseMap(diffuseMap);
		//}
		delete diffuseMap;
	}

	void Mesh::SetNormal(const std::string& normal)
	{
		Texture* normalMap = Texture::LoadFromFile(m_pDevice, normal);
		m_pEffectBase->SetNormalMap(normalMap);
		//if (m_pEffectBase)
		//{
		//	m_pEffectBase->SetDiffuseMap(normalMap);
		//}
		delete normalMap;
	}

	void Mesh::SetSpecular(const std::string& specular)
	{
		Texture* specularMap = Texture::LoadFromFile(m_pDevice, specular);
		m_pEffectBase->SetSpecularMap(specularMap);
		//if (m_pEffectBase)
		//{
		//	m_pEffectBase->SetDiffuseMap(specularMap);
		//}
		delete specularMap;
	}

	void Mesh::SetGloss(const std::string& gloss)
	{
		Texture* glossinessMap = Texture::LoadFromFile(m_pDevice, gloss);
		m_pEffectBase->SetGlossinessMap(glossinessMap);
		/*if (m_pEffectBase)
		{
			m_pEffectBase->SetDiffuseMap(glossinessMap);
		}*/
		delete glossinessMap;
	}

	std::vector<Vertex> Mesh::GetVertices() const
	{
		return m_Vertices;
	}

	std::vector<uint32_t> Mesh::GetIndices() const
	{
		return m_Indices;
	}

	EffectBase* Mesh::GetEffect() const
	{
		return m_pEffectBase;
	}
}