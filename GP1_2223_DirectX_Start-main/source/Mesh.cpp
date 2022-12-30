#include "pch.h"
#include "Mesh.h"
#include <assert.h>




namespace dae
{
	Mesh::Mesh(ID3D11Device* pDevice, const std::string& vehicle) :
		m_pDevice{pDevice}
	{
		std::vector<Vertex_PosCol>		vertices{};
		std::vector<uint32_t>	indices{};
		ParseOBJ(vehicle, vertices, indices);

		m_pEffect = new Effect(pDevice, L"Resources/PosCol3D.fx") ;
		
		//Create vertex buffer
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = vertices.data();

		HRESULT result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result))
			return;

		//Create index buffer
		m_NumIndices = static_cast<uint32_t>(indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		initData.pSysMem = indices.data();
		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
		if (FAILED(result))
			return;
	}

	Mesh::~Mesh()
	{
		delete m_pEffect;
	}

	void Mesh::Update(const Timer* pTimer, Camera* camera)
	{
		m_pEffect->ToggleSamplerTechnique();
		m_Yawn += pTimer->GetElapsed() * m_RotationSpeed;
		Matrix rotationMatrix = Matrix::CreateRotationY(m_Yawn);


		//set the matrix for the PosCol3D
		Matrix worldViewProjectionMatrix = rotationMatrix * camera->GetWorldViewProjectionMatrix();
		float* arrayWorldViewMatrix = reinterpret_cast<float*>(&worldViewProjectionMatrix);
		m_pEffect->GetMatWorldViewProjVariable()->SetMatrix(arrayWorldViewMatrix);

		//set the world  matrix for PosCol3D
		Matrix worldMatrix = rotationMatrix * camera->GetViewMatrix();
		float* arrayWorldMatrix = reinterpret_cast<float*>(&worldMatrix);
		m_pEffect->GetMatWorldMatrixVariable()->SetMatrix(arrayWorldMatrix);

		//set the inverse view matrix
		Matrix inverseViewMatrix = rotationMatrix * camera->GetInvViewMatrix();
		float* arrayInverseViewMatrix = reinterpret_cast<float*>(&inverseViewMatrix);
		m_pEffect->GetMatInvViewMatrixVariable()->SetMatrix(arrayInverseViewMatrix);
	}

	void Mesh::Render(ID3D11DeviceContext* pDeviceContext) const
	{

		////1. Set Primative Topology
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		//2. Set Input Layout
		pDeviceContext->IASetInputLayout(m_pEffect->GetInputLayout());

		//3. SetVertexBuffer
		constexpr UINT stride = sizeof(Vertex_PosCol);
		constexpr UINT offset = 0;

		ID3D11Buffer* pVertexBuffer = m_pVertexBuffer;

		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		//4. set indexbuffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//5. Draw
		//renderTriangle
		D3DX11_TECHNIQUE_DESC techDesc{};
		m_pEffect->GetTechniquePointer()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pEffect->GetTechniquePointer()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
	}

	void Mesh::SetTextures(const std::string& diffuse, const std::string& normal, const std::string& specular, const std::string& gloss) const
	{
		if (diffuse.size() > 0)
		{
			Texture* diffuseMap = Texture::LoadFromFile(m_pDevice, diffuse);
			m_pEffect->SetDiffuseMap(diffuseMap);
			delete diffuseMap;
		}
		if (normal.size() > 0)
		{
			Texture* normalMap = Texture::LoadFromFile(m_pDevice, normal);
			m_pEffect->SetNormalMap(normalMap);
			delete normalMap;
		}
		if (specular.size() > 0)
		{
			Texture* specularMap = Texture::LoadFromFile(m_pDevice, specular);
			m_pEffect->SetSpecularMap(specularMap);
			delete specularMap;
		}
		if (gloss.size() > 0)
		{
			Texture* glossinessMap = Texture::LoadFromFile(m_pDevice, gloss);
			m_pEffect->SetGlossinessMap(glossinessMap);
			delete glossinessMap;
		}
	}

	void Mesh::ToggleTextures() const
	{
		//
	}
}