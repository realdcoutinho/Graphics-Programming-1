#pragma once
#include <vector>
#include "Effect.h"


namespace dae
{
	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, const std::string& vehicle);
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) noexcept = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = delete;


		void Update(const Timer* pTimer, Camera *camera);
		void Render(ID3D11DeviceContext* pDeviceContext) const;
		void ToggleTextures() const;

		void SetTextures(const std::string& diffuse, const std::string& normal, const std::string& specular, const std::string& gloss) const;

	private:

		float m_Yawn;
		float m_RotationSpeed{ 1.0f };

		Effect* m_pEffect;

		ID3D11Device* m_pDevice;
		//std::vector<Vertex_Input> m_Vertices;
		std::vector<Vertex_PosCol> m_Vertices;
		std::vector<uint32_t> m_Indices;
		uint32_t m_NumIndices;

		ID3DX11EffectTechnique* m_pTechnique;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;



	};
}

