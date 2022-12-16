#pragma once

#include <cstdint>
#include <vector>

#include "Camera.h"
#include "DataTypes.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	//foward declarations
	class Texture;
	struct Mesh;
	struct Vertex;
	class Timer;
	class Scene;
	struct Light;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(Timer* pTimer);
		void Render();

#pragma region preiousRenders
		//void Render_W1_RasterizationOnly();
		//void Render_W1_ProjectionStage();
		//void Render_W1_BarycentricCoordinates();
		//void Render_W1_DepthBuffer();
		//void Render_W2_Quad();
#pragma endregion preiousRenders
		void Render_W3_TukTuk();
		void Render_W4_Vehicle();




		bool SaveBufferToImage() const;

		void ToggleDepthBuffer();
		void ToggleRotation();
		void ToggleShadingMode();
		void ToggleNormalMap() { m_IsNormalMapOn = !m_IsNormalMapOn; };
		void ToggleParallelFor() { m_IsParallelOn = !m_IsParallelOn; };


	private:
		enum class Mode
		{
			Texture,
			Buffer,
			Hit
		};
		enum class Rotation
		{
			Yes,
			No
		};
		enum class NormalMap
		{
			Yes,
			No
		};
		enum class ShadingMode
		{
			ObservedArea,
			Diffuse,
			Specular,
			Combined
		};


		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};
		
		Texture* m_pTexture;
		Texture* m_pNormalMap;
		Texture* m_pSpecular;
		Texture* m_pGloss;
		
		Light m_Light;

		float* m_pDepthBufferPixels{};


		Mode m_Mode = Mode::Texture;
		Rotation m_Rotation = Rotation::Yes;
		NormalMap m_Normal = NormalMap::Yes;
		ShadingMode m_ShadingMode = ShadingMode::Combined;

		Mesh m_MeshTuktuk;
		Mesh m_MeshVehicle;
		Camera m_Camera{};

		int m_Width{};
		int m_Height{};
		float m_AspectRatio{};
		float m_yawn{};
		float m_RotationSpeed{};

		bool m_IsNormalMapOn{ true };
		bool m_IsParallelOn{ true };

		//Function that transforms the vertices from the mesh from World space to Screen space
		void VertexTransformationFunction(std::vector<Mesh>& mesh) const; //W4 Version
		//void VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex>& vertices_out) const; //W1 Version
		//void VertexTransformationFunction(Mesh& mesh) const; //Wx Version

		//Initializations
		void InitializeTuktuk();
		void InitializeVehicle();

		//its not const on purpose
		ColorRGB PixelShading(const Vertex_Out& v) const;


		//Renderes the Triangle
		void RenderTriangle(Vertex_Out& v0, Vertex_Out& v1, Vertex_Out& v2);
		void RenderTriangleW4(Vertex_Out& v0, Vertex_Out& v1, Vertex_Out& v2);

#pragma region oldHelpFunctions
		//float sign(const Vector3& vertex1, const Vector3& vertex2, const Vector3& pixel) const;
		//bool PointInTriangle(const std::vector<Vertex_Out>& vertex, const Vector3& pixel) const;
		//float CalculateWeights(const Vector2& vertex1, const Vector2& vertex2, const Vector2& pixel, const float area) const;
		//float CalculateArea(const Vector2& v0, const Vector2& v1, const Vector2& v2) const;
		//bool SlabTest(const Vector3& minAABB, const Vector3& maxAABB, const Vector2& pixel) const;
		//void CalculateAABB(const std::vector<Vertex_Out>& vertices, Vector3& minAABB, Vector3& maxAABB) const;
		//float CalculateInterpolatedZ(const std::vector<Vertex_Out>& triangle, const float w0, const float w1, const float w2) const;
		//float CalculateInterpolatedW(const std::vector<Vertex_Out>& triangle, const float w0, const float w1, const float w2) const;
		//Vector2 CalculateInterpolatedUV(const std::vector<Vertex_Out>& triangle, const float w0, const float w1, const float w2, const float interpolatedW) const;
		//bool FrustumCulling(const Vertex_Out& vertex) const;
		//void TransformToRasterSpace(Vertex_Out& vertex) const;
		//float Remap(float value) const;
#pragma endregion oldHelpFunctions
	};
}