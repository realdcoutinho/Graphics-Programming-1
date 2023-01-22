#pragma once
#include <vector>
#include "EffectBase.h"
#include "EffectVehicle.h"
#include "FireEffect.h"
#include "Utils.h"

namespace dae
{
	class Mesh final
	{
	public:
		Mesh(const std::string& vehicle);
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) noexcept = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = delete;


		void Update(Camera camera, bool isToggling, float yawn);
		void Render(ID3D11DeviceContext* pDeviceContext) const;
		void ToggleTextures() const;

		void SetEffect(ID3D11Device* pDevice, const std::wstring& shader, const std::string& effect);
		void SetDiffuse(const std::string& diffuse);
		void SetNormal(const std::string& normal);
		void SetSpecular(const std::string& specular);
		void SetGloss(const std::string& gloss);


		void CreateBuffers(ID3D11Device* pDevice);

		std::vector<Vertex> GetVertices() const;
		std::vector<uint32_t> GetIndices() const;
		EffectBase* GetEffect() const;
		

		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		uint32_t m_NumIndices;

	private:
		float m_Yawn;
		float m_RotationSpeed{ 1.0f };

	/*	const uint8_t* m_pKeyboardState = SDL_GetKeyboardState(nullptr);*/

		//std::vector<Vertex> m_Vertices;
		//std::vector<uint32_t> m_Indices;
		//uint32_t m_NumIndices;


		EffectBase* m_pEffectBase;

		ID3D11Device* m_pDevice;
		ID3DX11EffectTechnique* m_pTechnique;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;



	};
}

