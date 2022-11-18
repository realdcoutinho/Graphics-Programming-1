//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Texture.h"
#include "Utils.h"

//my includes
#include <vector>

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow(pWindow)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	m_pDepthBufferPixels = new float[m_Width * m_Height];

	//Initialize Camera
	m_Camera.Initialize(60.f, { .0f,.0f,-10.f });
	m_Camera.CalculateViewMatrix();
}

Renderer::~Renderer()
{
	delete[] m_pDepthBufferPixels;
}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
}

inline float CalculateWeights(const Vector2& vertex1, const Vector2& vertex2, const Vector2& pixel, float area)
{
	return Vector2::Cross(vertex2 - vertex1, pixel - vertex1) / area;
}



void Renderer::Render_W1_RasterizationOnly()
{

	std::vector<Vertex> vertices_ndc
	{
		{{0.0f, 0.5f, 1.0f}},
		{{0.5f, -0.5f, 1.0f}},
		{{-0.5f, -0.5f, 1.0f}}
	};

	//Define Triangle - Vertices in NDC space
	//Triangle triangle = { Vector3(0.0f, 0.5f, 1.0f), Vector3(0.5f, -0.5f, 1.0f), Vector3(-0.5f, -0.5f, 1.0f) };
	//triangle.TransformToScreenSpace(static_cast<float>(m_Width), static_cast<float>(m_Height));


	for (auto& vertex : vertices_ndc)
	{
		vertex.position.x = (vertex.position.x + 1) / 2 * m_Width;
		vertex.position.y = (1 - vertex.position.y) / 2 * m_Height;
	}

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			ColorRGB finalColor{ 0, 0, 0 };

			Vector3 point{ static_cast<float>(px),  static_cast<float>(py), 0 };

			
			if (PointInTriangle(vertices_ndc, point))
			{
				finalColor = { 1,1,1 };
			}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}
}

void Renderer::Render_W1_ProjectionStage()
{
	std::vector<Vertex> vertices_ndc
	{ 
		{{0.0f, 2.0f, 0.0f}},
		{{1.0f, 0.0f, 0.0f}},
		{{-1.0f, 0.0f, 0.0f}}
	};


	std::vector<Vertex> vertices{};
	VertexTransformationFunction(vertices_ndc, vertices);

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			ColorRGB finalColor{ 0, 0, 0 };

			Vector3 point{ static_cast<float>(px),  static_cast<float>(py), 0 };

			if (PointInTriangle(vertices, point))
			{
				finalColor = { 1,1,1 };
			}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}
}

void Renderer::Render_W1_BarycentricCoordinates()
{
	//std::vector<Vertex> vertices_ndc
	//{
	//	{{0.0f, 4.0f, 2.0f}, {1, 0, 0}},
	//	{{3.0f, -2.0f, 2.0f}, {0, 1, 0}},
	//	{{-3.0f, -2.0f, 2.0f}, {0, 0, 1}}
	//};

	//std::vector<Vertex> vertices{};
	//VertexTransformationFunction(vertices_ndc, vertices);

	////float area = triangleArea(vertices);


	//for (int px{}; px < m_Width; ++px)
	//{
	//	for (int py{}; py < m_Height; ++py)
	//	{
	//		ColorRGB finalColor{ 0, 0, 0 };
	//		
	//		Vector3 point{ static_cast<float>(px),  static_cast<float>(py), 0 };

	//		if (PointInTriangle(vertices, point))
	//		{
	//			float w0 = CalculateWeights(vertices[1].position, vertices[2].position, point, area); //v1 and v2
	//			float w1 = CalculateWeights(vertices[2].position, vertices[0].position, point, area); //v2 and v0
	//			float w2 = CalculateWeights(vertices[0].position, vertices[1].position, point, area); //v1 and v0
	//			//float totalTriangleArea = (w0 + w1 + w2);
	//			//assert((totalTriangleArea == 1) && (w0 >= 0) && (w1 >= 0) && (w2 >= 0) && "We have a problem in the triangle");


	//			finalColor = { vertices[0].color * w0 + vertices[1].color * w1 + vertices[2].color * w2 };
	//		}

	//		//Update Color in Buffer
	//		finalColor.MaxToOne();

	//		m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
	//			static_cast<uint8_t>(finalColor.r * 255),
	//			static_cast<uint8_t>(finalColor.g * 255),
	//			static_cast<uint8_t>(finalColor.b * 255));
	//	}
	//}
}

void Renderer::Render_W1_DepthBuffer()
{
	std::vector<Vertex> vertices_ndc
	{
		//triangle1
		{{0.0f, 2.0f, 0.0f}, {1, 0, 0}},
		{{1.5f, -1.0f, 0.0f}, {1, 0, 0}},
		{{-1.5f, -1.0f, 0.0f}, {1, 0, 0}},

		////triangle 2
		{{0.0f, 4.0f, 2.0f}, {1, 0, 0}},
		{{3.0f, -2.0f, 2.0f}, {0, 1, 0}},
		{{-3.0f, -2.0f, 2.0f}, {0, 0, 1}}
	};

	std::vector<Vertex> vertices{};
	VertexTransformationFunction(vertices_ndc, vertices);

	//float area = triangleArea(vertices);

	std::fill_n(m_pDepthBufferPixels, m_Width * m_Height, FLT_MAX);
	SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));

	int vectorSize{ static_cast<int>(vertices.size()) };
	for (int index{}; index < vectorSize; index += 3)
	{
		std::vector<Vertex> triangle
		{
			vertices[index + 0],
			vertices[index + 1],
			vertices[index + 2],
		};

		Vector3 minAABB{};
		Vector3 maxAABB{};
		CalculateAABB(triangle, minAABB, maxAABB);

	
		const Vector2 v0{ triangle[0].position.x, triangle[0].position.y };
		const Vector2 v1{ triangle[1].position.x, triangle[1].position.y };
		const Vector2 v2{ triangle[2].position.x, triangle[2].position.y };
	
		float area{ CalculateArea(v0, v1, v2) };
	
		for (int px{}; px < m_Width; ++px)
		{
			for (int py{}; py < m_Height; ++py)
			{

				ColorRGB finalColor{ 0, 0, 0 };
	
				Vector2 point{ static_cast<float>(px),  static_cast<float>(py) };

				if (SlabTest(minAABB, maxAABB, point))
				{
					//float w0 = Vector2::Cross(v2 - v1, point - v1) / area;
					float w0{ CalculateWeights(v1, v2, point, area) };
					//float w1 = Vector2::Cross(v0 - v2, point - v2) / area;
					float w1{ CalculateWeights(v2, v0, point, area) };
					//float w2 = Vector2::Cross(v1 - v0, point - v0) / area;
					float w2{ CalculateWeights(v0, v1, point, area) };

					const Vector3 pixelPosition = vertices[index + 0].position * w0 + vertices[index + 1].position * w1 + vertices[index + 2].position * w2;;


					if (PointInTriangle(triangle, pixelPosition))
					{
						float depth = m_pDepthBufferPixels[py * m_Width + px];
						if (depth > pixelPosition.z)
						{
							m_pDepthBufferPixels[py * m_Width + px] = pixelPosition.z;
							finalColor = vertices[index].color * w0 + vertices[index + 1].color * w1 + vertices[index + 2].color * w2;
							//Update Color in Buffer
							finalColor.MaxToOne();

							m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
								static_cast<uint8_t>(finalColor.r * 255),
								static_cast<uint8_t>(finalColor.g * 255),
								static_cast<uint8_t>(finalColor.b * 255));

						}
					}
				}
			}
		}
	}
	
}


void Renderer::Render()
{
	m_AspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

	//@START
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);

	//Render_W1_RasterizationOnly();
	//Render_W1_ProjectionStage();
	//Render_W1_BarycentricCoordinates();
	Render_W1_DepthBuffer();

	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);

}

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex>& vertices_out) const
{
	//Todo > W1 Projection Stage

	vertices_out = vertices_in;
	Matrix viewMatrix = m_Camera.viewMatrix;


	for (auto& vertex : vertices_out)
	{
		//3 stepts required to transform a point from world space to NDC:

		//transform to camera space // apply matrix
		//1. transform all vertices from world space to camera space (view space)
		vertex.position = viewMatrix.TransformPoint(vertex.position); 

		//2.transform these vertices from the view space to the NDC(Normalized Device Coordinates) -> Prespective Divide
		//The 3D point is now in the Projection Space
		vertex.position.x = vertex.position.x / vertex.position.z;
		vertex.position.y = vertex.position.y / vertex.position.z; //camera settings

		//3. We need to consider the camera settings. Apply the aspect ratio and camera fov onto the vertex points.
		vertex.position.x = (vertex.position.x) / (m_AspectRatio * m_Camera.fov);
		vertex.position.y = (vertex.position.y) / m_Camera.fov;

		//As a final step, we place every vertex point on the screen space before comparing with the actual pixels
		vertex.position.x = (vertex.position.x + 1) / 2 * m_Width;
		vertex.position.y = (1 - vertex.position.y) / 2 * m_Height;
	}
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}

float Renderer::sign(const Vector3& vertex1, const Vector3& vertex2, const Vector3& pixel)
{
	return (pixel.x - vertex2.x) * (vertex1.y - vertex2.y) - (vertex1.x - vertex2.x) * (pixel.y - vertex2.y);
}

bool Renderer::PointInTriangle(const std::vector<Vertex>& vertex, const Vector3& pixel)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(vertex[0].position, vertex[1].position, pixel);
	d2 = sign(vertex[1].position, vertex[2].position, pixel);
	d3 = sign(vertex[2].position, vertex[0].position, pixel);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

float Renderer::CalculateArea(const Vector2& v0, const Vector2& v1, const Vector2& v2)
{
	return Vector2::Cross((v1 - v0), v2 - v0);
}

void Renderer::CalculateAABB(const std::vector<Vertex>& vertices, Vector3& minAABB, Vector3& maxAABB)
{

	minAABB = vertices[0].position;
	maxAABB = vertices[0].position;

	for (const Vertex& pos : vertices)
	{
		//get min
		if (pos.position.x < minAABB.x)
			minAABB.x = pos.position.x;
		if (pos.position.y < minAABB.y)
			minAABB.y = pos.position.y;
		if (pos.position.z < minAABB.z)
			minAABB.z = pos.position.z;

		//get max
		if (pos.position.x > maxAABB.x)
			maxAABB.x = pos.position.x;
		if (pos.position.y > maxAABB.y)
			maxAABB.y = pos.position.y;
		if (pos.position.z > maxAABB.z)
			maxAABB.z = pos.position.z;
	}

	Clamp(minAABB.x, 0.0f, static_cast<float>(m_Width) );
	Clamp(minAABB.y, 0.0f, static_cast<float>(m_Height));
	Clamp(minAABB.z, 0.0f, FLT_MAX); // ASK ABOUT THIS ONE
}

bool Renderer::SlabTest(const Vector3& minAABB, const Vector3& maxAABB, const Vector2& pixel) //NOT A BOUNDING BOX PER SAY. ASK IN CLASS
{
	//check X
	if (pixel.x < minAABB.x || pixel.x > maxAABB.x)
		return false;
	if (pixel.y < minAABB.y || pixel.y > maxAABB.y)
		return false;
	
	return true;
}

//void Renderer::GetVertexUpdate(const std::vector<Vertex>& vertices, const float w0, const float w1, const float w2, const int index = 0, const bool color = 0, const bool position = 0)
//{
//	//vertices[index].position
//
//	if (color)
//		vertices[index].color * w0 + vertices[index + 1].color * w1 + vertices[index + 2].color * w2;
//	if (position)
//		vertices[index + 0].position* w0 + vertices[index + 1].position * w1 + vertices[index + 2].position * w2;
//}
//
//Vector3 Renderer::GetPositionFromWeights(const std::vector<Vertex>& vertices, float w0, float w1, float w2, int index = 0)
//{
//	std::vector<Vertex> clone = vertices;
//	return clone[index + 0].position* w0 + clone[index + 1].position * w1 + clone[index + 2].position * w2;
//}
//
//ColorRGB Renderer::GetColorFromWeights(const std::vector<Vertex>& vertices, float w0, float w1, float w2, int index = 0)
//{
//	std::vector<Vertex> clone = vertices;
//	return clone[index].color* w0 + clone[index + 1].color * w1 + clone[index + 2].color * w2;
//}