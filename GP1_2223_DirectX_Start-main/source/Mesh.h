#pragma once
#include <vector>
#include "Effect.h"


namespace dae
{
	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices);
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) noexcept = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = delete;

		void Render(ID3D11DeviceContext* pDeviceContext) const;

	private:
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

