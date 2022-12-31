#pragma once
#include <vector>
#include "Effect.h"
#include "EffectBase.h"
#include "EffectVehicle.h"

namespace dae
{
	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, const std::string& vehicle, const std::wstring& shader);
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) noexcept = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = delete;


		void Update(const Timer* pTimer, Camera *camera);
		void Render(ID3D11DeviceContext* pDeviceContext) const;
		void ToggleTextures() const;

		void SetDiffuse(const std::string& diffuse);
		void SetNormal(const std::string& normal);
		void SetSpecular(const std::string& specular);
		void SetGloss(const std::string& gloss);

	private:

		float m_Yawn;
		float m_RotationSpeed{ 1.0f };

		//Effect* m_pEffect;
		EffectVehicle* m_pEffectVehicle;

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

