//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "pch.h"
#include "Render_Software.h"
#include "Math.h"
#include "Matrix.h"
#include "Texture.h"
#include "Utils.h"

//my includes
#include <vector>
#include <ppl.h>

using namespace dae;
using namespace Utils;

	Render_Software::Render_Software(SDL_Window* pWindow, Camera& camera, Mesh* mesh) :
		Render_Base(pWindow, camera, mesh),
		 m_IsNormalMapOn{ true },
		 m_IsParallelOn{ true },
		 m_IsDepthBufferOn{ false },
		 m_IsBoundingBoxOn{ false },
		 m_IsTextureOn{ true }
	{
		CreateBuffers(pWindow);
		InitializeTextures();
		InitializeLights();

		//std::cout << "Software Rasterizer Initialized and ready!" << '\n';

	}
	void Render_Software::CreateBuffers(SDL_Window* pWindow)
	{
		//Create Buffers
		m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
		m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
		m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
		m_pDepthBufferPixels = new float[m_Width * m_Height];
	}

	Render_Software::~Render_Software()
	{
		delete[] m_pDepthBufferPixels;
		delete m_pDiffuse;
		delete m_pNormalMap;
		delete m_pSpecular;
		delete m_pGloss;
	}

	void Render_Software::Update(const Timer* pTimer)
	{
		UpdateCamera(pTimer);
		UpdateYawn(pTimer);
	}

	void Render_Software::Render() const
	{
		std::fill_n(m_pDepthBufferPixels, m_Width * m_Height, FLT_MAX);

		m_pBackBuffer->format;
		if(m_IsUniformColorOn)
			SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format, static_cast<Uint8>(25.5f), static_cast<Uint8>(25.5f), static_cast<Uint8>(25.5f)));
		else
		{
			SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format, static_cast<Uint8>(99.45f) , static_cast<Uint8>(99.45f), static_cast<Uint8>(99.45f)));
		}

		//@START
		//Lock BackBuffer
		SDL_LockSurface(m_pBackBuffer);

		Render_Vehicle();

		//@END
		//Update SDL Surface
		SDL_UnlockSurface(m_pBackBuffer);
		SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
		SDL_UpdateWindowSurface(m_pWindow);
	}

	void Render_Software::Render_Vehicle() const
	{
		std::vector<MeshStruct> meshes
		{
			m_MeshVehicle
		};
	
		VertexTransformationFunction(meshes);
		for (const MeshStruct& mesh : meshes)
		{
			
			const uint32_t amountOfTriangles = (static_cast<uint32_t>(m_pVehicleMesh->m_Indices.size()) / 3);
			concurrency::parallel_for(static_cast<uint32_t>(0), amountOfTriangles, [=, this](int index)
			{
				Vertex_Out v0 = mesh.vertices_out[m_pVehicleMesh->m_Indices[3 * index]];
				Vertex_Out v1 = mesh.vertices_out[m_pVehicleMesh->m_Indices[3 * index + 1]];
				Vertex_Out v2 = mesh.vertices_out[m_pVehicleMesh->m_Indices[3 * index + 2]];
	
				RenderTriangleW4(v0, v1, v2);
			});
		}
	}


void Render_Software::VertexTransformationFunction(std::vector<MeshStruct>& meshes) const //W2 Version
{
	//calculate the normals with the world matrix only, never with the world view matrix
	//dont do a prepective divide onto the normals -> "only do it in on the position" -> hmmm what?
	//we only need the position
	//always normalize the interpolated result from the normal vectors


	Matrix worlViewProjectionMatrix;
	for (auto& mesh : meshes)
	{
		mesh.worldMatrix = Matrix::CreateRotationY(m_Yawn) * Matrix::CreateTranslation({ 0.0f, 0.0f, 50.0f });
		worlViewProjectionMatrix = mesh.worldMatrix * m_Camera.worldViewProjectionMatrix;

		mesh.vertices_out.clear();
		for (auto& vertex : m_pVehicleMesh->GetVertices())
		{
			//okay, so here I am transforming the normals of each vertice
			vertex.normal = mesh.worldMatrix.TransformVector(vertex.normal).Normalized();
			//and here I transform the tangents of each vertex with the world matrix
			vertex.tangent = mesh.worldMatrix.TransformVector(vertex.tangent).Normalized();
			//and now you can create the view direction
			
			
			//my calculation
			vertex.viewDirection = (worlViewProjectionMatrix.TransformPoint(vertex.position) - m_Camera.origin).Normalized();
			
			//Rutgers calculation
			//vertex.viewDirection = worlViewProjectionMatrix.TransformPoint(vertex.position) - m_Camera.origin;

			//3 stepts required to transform a point from world space to NDC:

			//Create a Vector4 from the original vertex Position adding 1 to the w value
			Vector4 vertexPosition{ vertex.position.x,  vertex.position.y,  vertex.position.z, 1 };
			//transform to camera space // apply matrix

			//1. transform all vertices from world space to camera space (view space)
			vertexPosition = worlViewProjectionMatrix.TransformPoint(vertexPosition);

			//2. apply prespective devide
			vertexPosition.w = vertexPosition.w;
			vertexPosition.x = vertexPosition.x / vertexPosition.w;
			vertexPosition.y = vertexPosition.y / vertexPosition.w;
			vertexPosition.z = vertexPosition.z / vertexPosition.w;
			//4. create a vertex out from the previous calculations and using vertex color and uv
			const Vertex_Out newVertexOut{ vertexPosition, vertex.color, vertex.uv, vertex.normal, vertex.tangent, vertex.viewDirection };
			//5. place thme in the vector
			mesh.vertices_out.emplace_back(newVertexOut);

			////2.transform these vertices from the view space to the NDC(Normalized Device Coordinates) -> Prespective Divide
			////The 3D point is now in the Projection Space
			//vertex.position.x = vertex.position.x / vertex.position.z;
			//vertex.position.y = vertex.position.y / vertex.position.z; //camera settings
			//
			////3. We need to consider the camera settings. Apply the aspect ratio and camera fov onto the vertex points.
			//vertex.position.x = (vertex.position.x) / (m_AspectRatio * m_Camera.fov);
			//vertex.position.y = (vertex.position.y) / m_Camera.fov;
			//
			////As a final step, we place every vertex point on the screen space before comparing with the actual pixels
			//vertex.position.x = (vertex.position.x + 1) / 2 * m_Width;
			//vertex.position.y = (1 - vertex.position.y) / 2 * m_Height;
		}
	}
}


void Render_Software::RenderTriangleW4(Vertex_Out& v0, Vertex_Out& v1, Vertex_Out& v2) const 
{
	if ((FrustumCulling(v0)) || (FrustumCulling(v1)) || (FrustumCulling(v2)))
		return;

	Vertex_Out V0_Out = v0;
	Vertex_Out V1_Out = v1;
	Vertex_Out V2_Out = v2;

	TransformToRasterSpace(V0_Out, V1_Out, V2_Out, m_Width, m_Height);

	std::vector<Vertex_Out> triangle
	{
		V0_Out,
		V1_Out,
		V2_Out,
	};

	Vector3 minAABB{};
	Vector3 maxAABB{};
	CalculateAABB(triangle, minAABB, maxAABB, m_Width, m_Height);

	const Vector2 V0{ V0_Out.position.x, V0_Out.position.y };
	const Vector2 V1{ V1_Out.position.x, V1_Out.position.y };
	const Vector2 V2{ V2_Out.position.x, V2_Out.position.y };

	float area{ CalculateArea(V0, V1, V2) };

	switch (m_CullMode)
	{
	case CullMode::Front:
		if (area <= 0) // if area is negative, we are looking at the back of the triangle, hence, do not render
			return;
		break;
	case CullMode::Back:
		if (area >= 0) // if area is negative, we are looking at the back of the triangle, hence, do not render
			return;
		break;
	case CullMode::none:
		//if (area <= 0) // if area is negative, we are looking at the back of the triangle, hence, do not render
		//	return;
		break;
	}


	ColorRGB finalColor{ colors::Black };

	for (int px{ static_cast<int>(minAABB.x) }; px < static_cast<int>(maxAABB.x); ++px)
	{
		for (int py{ static_cast<int>(minAABB.y) }; py < static_cast<int>(maxAABB.y); ++py)
		{

			if (m_IsBoundingBoxOn)
			{
				finalColor = { colors::White };
				finalColor.MaxToOne();

				m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
					static_cast<uint8_t>(finalColor.r * 255),
					static_cast<uint8_t>(finalColor.g * 255),
					static_cast<uint8_t>(finalColor.b * 255));
				continue;
			}

			Vector2 point{ static_cast<float>(px) + 0.5f,  static_cast<float>(py) + 0.5f };

			//float w0 = Vector2::Cross(v2 - v1, point - v1) / area;
			float w0{ CalculateWeights(V1, V2, point, area) };
			//float w1 = Vector2::Cross(v0 - v2, point - v2) / area;
			float w1{ CalculateWeights(V2, V0, point, area) };
			//float w2 = Vector2::Cross(v1 - v0, point - v0) / area;
			float w2{ CalculateWeights(V0, V1, point, area) };

			const bool isInTriangle = w0 > 0 && w1 > 0 && w2 > 0;
			if (isInTriangle)
			{
				const float zInterpolated{ CalculateInterpolatedZ(triangle, w0, w1, w2) };
				const int currentPixel{ py * m_Width + px };
				const float depth = m_pDepthBufferPixels[currentPixel];

				//Z culling
				if (zInterpolated < 0.0f || zInterpolated > 1.0f)
					continue;

				if (m_IsTextureOn)
				{
					if (depth > zInterpolated)
					{
						m_pDepthBufferPixels[currentPixel] = zInterpolated;
						const float wInterpolated{ CalculateInterpolatedW(triangle, w0, w1, w2) };

						//I dont make my vertex_out const because I change color
						const Vertex_Out pixelVertexPos{ CalculateInterpolatedVerTex_Out(triangle, w0, w1, w2, wInterpolated) };
						finalColor = PixelShading(pixelVertexPos);

						finalColor.MaxToOne();

						m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(finalColor.r * 255),
							static_cast<uint8_t>(finalColor.g * 255),
							static_cast<uint8_t>(finalColor.b * 255));
					}
				}
				if (m_IsDepthBufferOn)
				{
					if (depth > zInterpolated)
					{
						m_pDepthBufferPixels[currentPixel] = zInterpolated;
						const float remapValue = Utils::Remap(m_pDepthBufferPixels[currentPixel]);

						m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(remapValue * 255),
							static_cast<uint8_t>(remapValue * 255),
							static_cast<uint8_t>(remapValue * 255));
					}
				}
			}
		}
	}
}

ColorRGB Render_Software::PixelShading(const Vertex_Out& v) const
{
#pragma region normals

	//calculate normals
	float lambertCosine{ Vector3::Dot(v.normal, -m_Light.direction) }; //aka ObservedArea with no normal
	if (lambertCosine < 0)
		return { colors::Black };  // Skip if observedArea is negative
	if (m_IsNormalMapOn)
	{
		const Vector3 biNormal{ Vector3::Cross(v.normal, v.tangent).Normalized() };
		const Matrix tangentSpaceAxis{ v.tangent, biNormal, v.normal, Vector3::Zero };
		//sample normals
		const ColorRGB sampleNormal{ m_pNormalMap->Sample(v.uv) };
		Vector3 sampleNormalVector{ sampleNormal.r, sampleNormal.g, sampleNormal.b };
		//different order than that of the slides
		sampleNormalVector = 2.0f * sampleNormalVector - Vector3{ 1.0f, 1.0f, 1.0f };
		sampleNormalVector /= 255.0f;
		sampleNormalVector = tangentSpaceAxis.TransformVector(sampleNormalVector).Normalized();
		lambertCosine = Vector3::Dot(sampleNormalVector, -m_Light.direction); //aka ObservedArea
		if (lambertCosine < 0)
			return { colors::Black };  // Skip if observedArea is negative
	}

	
	//Sample Texture Map
	const ColorRGB sampleTextureMap{ m_pDiffuse->Sample(v.uv) };
	const ColorRGB diffuse{ ColorRGB{ Utils::Lambert(m_Light.intensity, sampleTextureMap) } };

	//Sample specular map
	const ColorRGB specularMap{ m_pSpecular->Sample(v.uv) };

	//Sample GLoss map
	const ColorRGB glossMap{ m_pGloss->Sample(v.uv) };

	//create a phong exponent
	const float phongExponent = glossMap.r * m_Light.shininess;

	//create a phong material
	ColorRGB phong{ Utils::Phong(specularMap, phongExponent, m_Light.direction, -v.viewDirection, v.normal)};

	switch (m_ShadingMode)
	{
	case ShadingMode::Combined:
		return { (m_Light.ambient + diffuse + phong) * lambertCosine };
		break;
	case  ShadingMode::ObservedArea:
		return { lambertCosine, lambertCosine, lambertCosine };
		break;
	case  ShadingMode::Diffuse:
		return diffuse * lambertCosine;
		break;
	case  ShadingMode::Specular:
		return phong * lambertCosine;
		break;
	}
	return {};
}


void Render_Software::InitializeLights()
{
	//lights
	const float		lightIntensity{ 7.0f };
	const float		lightShininess{ 25.0f };
	const Vector3	lighDirection{ 0.577f, -0.577f, 0.577f };
	const ColorRGB	ambient{ 0.025f, 0.025f, 0.025f };

	m_Light = AddDirectionalLight(lighDirection, lightIntensity, colors::White, lightShininess, ambient);
}

void Render_Software::InitializeTextures()
{
	////textures
	m_pDiffuse = Texture::LoadFromFile(m_DiffusePath);
	m_pNormalMap = Texture::LoadFromFile(m_NormalPath);
	m_pSpecular = Texture::LoadFromFile(m_SpecularPath);
	m_pGloss = Texture::LoadFromFile(m_GlossPath);
}

void Render_Software::ToggleShadingMode()
{
	HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, 5);

	switch (m_ShadingMode)
	{
	case ShadingMode::Combined:
		m_ShadingMode = ShadingMode::ObservedArea;
		std::cout << "**(SOFTWARE) Shading Mode = OBSERVED_AREA" << '\n';
		break;
	case ShadingMode::ObservedArea:
		m_ShadingMode = ShadingMode::Diffuse;
		std::cout << "**(SOFTWARE) Shading Mode = DIFFUSE" << '\n';
		break;
	case ShadingMode::Diffuse:
		m_ShadingMode = ShadingMode::Specular;
		std::cout << "**(SOFTWARE) Shading Mode = SPECULAR" << '\n';
		break;
	case ShadingMode::Specular:
		m_ShadingMode = ShadingMode::Combined;
		std::cout << "**(SOFTWARE) Shading Mode = COMBINED" << '\n';
		break;
		}
}

bool Render_Software::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}

void Render_Software::ToggleCullMode()
{
	switch (m_CullMode)
	{
	case CullMode::Front:
		m_CullMode = CullMode::Back;
		break;
	case CullMode::Back:
		m_CullMode = CullMode::none;
		break;
	case CullMode::none:
		m_CullMode = CullMode::Front;
		break;
	}

}

void Render_Software::ToggleDepthBuffer()
{
	HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, 5);

	if (!m_IsBoundingBoxOn)
	{
		m_IsDepthBufferOn = !m_IsDepthBufferOn;
		std::cout << "**(SOFTWARE) DepthBuffer Visualization On" << '\n';

		return;
	}
	m_IsBoundingBoxOn = !m_IsBoundingBoxOn;
	m_IsDepthBufferOn = !m_IsDepthBufferOn;
}

void Render_Software::ToggleBoundingBox()
{
	HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, 5);

	if (!m_IsDepthBufferOn)
	{
		m_IsBoundingBoxOn = !m_IsBoundingBoxOn;
		std::cout << "**(SOFTWARE) BoundingBox Visualization On" << '\n';

		return;
	}
	m_IsDepthBufferOn = !m_IsDepthBufferOn;
	m_IsBoundingBoxOn = !m_IsBoundingBoxOn;
}

void Render_Software::ToggleNormalMap()
{
	m_IsNormalMapOn = !m_IsNormalMapOn;
	HANDLE  h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, 5);

	if (m_IsNormalMapOn)
	{
		std::cout << "**(SOFTWARE) NormalMap On" << '\n';
	}
	if (!m_IsNormalMapOn)
	{

		std::cout << "**(SHARED) NormalMap Off" << '\n';

	}
}