#pragma once

#include <cstdint>
#include <vector>

#include "Camera.h"
#include "DataTypes.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Texture;
	struct Mesh;
	struct Vertex;
	class Timer;
	class Scene;

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

		void Render_W1_RasterizationOnly();
		void Render_W1_ProjectionStage();
		void Render_W1_BarycentricCoordinates();
		void Render_W1_DepthBuffer();


		bool SaveBufferToImage() const;
		//float CalculateWeights(const Vector3& vertex1, const Vector3& vertex2, const Vector3& pixel);
		//float CalculateWeights( Vector2& vertex1,  Vector2& vertex2,  Vector2& pixel);

		


	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		float* m_pDepthBufferPixels{};

		float sign(const Vector3& vertex1, const Vector3& vertex2, const Vector3& pixel);
		bool PointInTriangle(const std::vector<Vertex>& vertex, const Vector3& pixel);
		float CalculateArea(const Vector2& v0, const Vector2& v1, const Vector2& v2);
		bool SlabTest(const Vector3& minAABB, const Vector3& maxAABB, const Vector2& pixel);
		void CalculateAABB(const std::vector<Vertex>& vertices, Vector3& minAABB, Vector3& maxAABB);
		////void GetVertexUpdate(const std::vector<Vertex>& vertices, const float w0, const float w1, const float w2, const int index = 0, const bool color = 0, const bool position = 0);
		///
		//Vector3 GetPositionFromWeights(const std::vector<Vertex>& vertices, float w0, float w1, float w2, int index = 0);
		//ColorRGB GetColorFromWeights(const std::vector<Vertex>& vertices, float w0, float w1, float w2, int index = 0);

		Camera m_Camera{};

		int m_Width{};
		int m_Height{};
		float m_AspectRatio{};

		//Function that transforms the vertices from the mesh from World space to Screen space
		void VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex>& vertices_out) const; //W1 Version
	};
}


//for (size_t index{}; index < vertices.size(); index += 3)
//{
//	std::vector<Vertex> triangle
//	{
//		vertices[index + 0],
//		vertices[index + 1],
//		vertices[index + 2],
//	};
//
//	const Vector2 v0{ triangle[0].position.x, triangle[0].position.y };
//	const Vector2 v1{ triangle[1].position.x, triangle[1].position.y };
//	const Vector2 v2{ triangle[2].position.x, triangle[2].position.y };
//
//	float area = { Vector2::Cross((v1 - v0), v2 - v0) };
//
//	for (int px{}; px < m_Width; ++px)
//	{
//		for (int py{}; py < m_Height; ++py)
//		{
//			ColorRGB finalColor{ 0, 0, 0 };
//
//			Vector2 point{ static_cast<float>(px),  static_cast<float>(py) };
//			//Vector3 point{ static_cast<float>(px),  static_cast<float>(py), 0 };
//
//			//float w0 = CalculateWeights(triangle[index + 1].position, triangle[index + 2].position, point, area); //v1 and v2
//			//float w1 = CalculateWeights(triangle[index + 2].position, triangle[index + 0].position, point, area); //v2 and v0
//			//float w2 = CalculateWeights(triangle[index + 0].position, triangle[index + 1].position, point, area); //v1 and v0
//
//
//			float w0 = Vector2::Cross(v2 - v1, point - v1) / area;
//			float w1 = Vector2::Cross(v0 - v2, point - v2) / area;
//			float w2 = Vector2::Cross(v1 - v0, point - v0) / area;
//
//
//
//			const Vector3 pixelPosition = vertices[index + 0].position * w0 + vertices[index + 1].position * w1 + vertices[index + 2].position * w2;
//
//
//			if (PointInTriangle(triangle, pixelPosition))
//			{
//				//for (size_t index{}; index < vertices.size(); index += 3)
//				//{
//
//					//float totalTriangleArea = (w0 + w1 + w2);
//					//assert((totalTriangleArea == 1) && (w0 >= 0) && (w1 >= 0) && (w2 >= 0) && "We have a problem in the triangle");
//
//				float depth = m_pDepthBufferPixels[py * m_Width + px];
//				if (depth > pixelPosition.z)
//				{
//					m_pDepthBufferPixels[py * m_Width + px] = pixelPosition.z;
//					finalColor = { vertices[index + 0].color * w0 + vertices[index + 1].color * w1 + vertices[index + 2].color * w2 };
//					//Update Color in Buffer
//					finalColor.MaxToOne();
//
//					m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
//						static_cast<uint8_t>(finalColor.r * 255),
//						static_cast<uint8_t>(finalColor.g * 255),
//						static_cast<uint8_t>(finalColor.b * 255));
//
//				}
//				//}
//			}
//
//
//		}
//	}
//}