//#pragma once
//#include "pch.h"
//#include "Texture.h"
//
//namespace dae
//{
//	class Effect 
//	{
//	public:
//		Effect(ID3D11Device* pDevice, const std::wstring& assetFile); //assetFile == path that determines which effect to load
//		~Effect();
//
//		Effect(const Effect&) = delete;
//		Effect(Effect&&) noexcept = delete;
//		Effect& operator=(const Effect&) = delete;
//		Effect& operator=(Effect&&) noexcept = delete;
//
//		ID3DX11Effect* GetEffectPointer() const;
//		ID3DX11EffectTechnique* GetTechniquePointer() const;
//		ID3D11InputLayout* GetInputLayout() const;
//		ID3DX11EffectMatrixVariable* GetMatWorldViewProjVariable() const;
//		ID3DX11EffectMatrixVariable* GetMatWorldMatrixVariable() const;
//		ID3DX11EffectMatrixVariable* GetMatInvViewMatrixVariable() const;
//
//
//		void SetDiffuseMap(Texture* pDiffuseTexture);
//		void SetNormalMap(Texture* pTextureMap);
//		void SetSpecularMap(Texture* pTextureMap);
//		void SetGlossinessMap(Texture* pTextureMap);
//
//
//
//		//void SetSamplerState(D3D11_SAMPLER_DESC samplerState);
//		void ToggleSamplerTechnique();
//		void UpdateSamplerTechnique();
//
//
//	private:
//		
//		void SetShaderResources();
//
//		enum class samplerTechnique
//		{
//			Point, 
//			Linear,
//			Anisotropic
//		};
//
//		samplerTechnique m_SamplerTechnique = samplerTechnique::Point;
//		
//		ID3D11Device* m_pDevice{};
//		const std::wstring& m_AssetFile{};
//
//
//		ID3DX11Effect* m_pEffect{};
//		ID3DX11EffectTechnique* m_pTechnique{};
//		ID3D11InputLayout* m_pInputLayout;
//		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable; //WORLD VIEW PROJECTION MATRIX 
//		ID3DX11EffectMatrixVariable* m_pMatWorldMatrixVariable; // WORLD MATRIX
//		ID3DX11EffectMatrixVariable* m_pMatInverseViewMatrixVariable; //inverse view matrix
//
//
//		ID3DX11EffectSamplerVariable* m_pEffectSamplerVariable;
//		
//		//for Textures
//		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
//		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
//		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
//		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;
//
//	};
//
//	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
//}