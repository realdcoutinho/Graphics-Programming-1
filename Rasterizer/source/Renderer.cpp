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

	//m_pDepthBufferPixels = new float[m_Width * m_Height];

	//Initialize Camera
	m_Camera.Initialize(60.f, { .0f,.0f,-10.f });
	m_Camera.CalculateViewMatrix();
}

Renderer::~Renderer()
{
	//delete[] m_pDepthBufferPixels;
}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
}

void Renderer::Render_W1_RasterizationOnly()
{

	//Define Triangle - Vertices in NDC space
	Triangle triangle = { Vector3(0.0f, 0.5f, 1.0f), Vector3(0.5f, -0.5f, 1.0f), Vector3(-0.5f, -0.5f, 1.0f) };
	triangle.TransformToScreenSpace(m_Width, m_Height);


	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			ColorRGB finalColor{ 0, 0, 0 };

			Vector3 point{ static_cast<float>(px),  static_cast<float>(py), 0 };

			
			if (PointInTriangle(triangle, point))
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

void Renderer::Render_W2_ProjectionStage()
{
	//Triangle triangle = { Vector3(0.0f, 2.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f) };
	//triangle.TransformToCameraSpace(m_Camera.invViewMatrix);
	//triangle.TransformToScreenSpace(static_cast<float>(m_Width), static_cast<float>(m_Height));
	////triangle.TransformToProjectedStage();
	//m_Camera.CalculateViewMatrix();
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


void Renderer::Render()
{

	//@START
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);

	//Render_W1_RasterizationOnly();
	Render_W2_ProjectionStage();

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
	for (auto& vertex : vertices_out) //transform to camera space // apply matrix
	{
		vertex.position = viewMatrix.TransformPoint(vertex.position);
	}

	for (auto& vertex : vertices_out)
	{
		vertex.position.x = vertex.position.x / vertex.position.z;
		vertex.position.y = vertex.position.y / vertex.position.z;

		//camera settings
	}

	for (auto& vertex : vertices_out)
	{
		vertex.position.x = (vertex.position.x + 1) / 2 * m_Width;
		vertex.position.y = (1 - vertex.position.y) / 2 * m_Height;
	}
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}





float Renderer::sign(const Vector3& vextex1, const Vector3& vextex2, const Vector3& pixel)
{
	return (pixel.x - vextex2.x) * (vextex1.y - vextex2.y) - (vextex1.x - vextex2.x) * (pixel.y - vextex2.y);
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

bool Renderer::PointInTriangle(const Triangle& triangle, const Vector3& pixel)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(triangle.v0, triangle.v1, pixel);
	d2 = sign(triangle.v1, triangle.v2, pixel);
	d3 = sign(triangle.v2, triangle.v0, pixel);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

