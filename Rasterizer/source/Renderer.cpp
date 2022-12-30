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
#include <ppl.h>

using namespace dae;
using namespace Utils;

//What is still there to be fixed?
//1 the materials 
	//1.1 specific ally the phong are not working. 
	//1.2 the normals seem a bit off
	//1.3 the combination of all of them without the normals brings artifacts
//2. the planes
	//2.1 the near and far planes calculations seem a bit off. Triangles get culled way too soon and some dont
	//2.2 with the depth buffer remap thats more noticeble
//3. the main texture sometimes breaks

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
	m_AspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

	//InitializeTuktuk();
	InitializeVehicle();
}

Renderer::~Renderer()
{
	delete[] m_pDepthBufferPixels;
	delete m_pTexture;
	delete m_pNormalMap;
	delete m_pSpecular;
	delete m_pGloss;
}

void Renderer::InitializeVehicle()
{
	//variable
	m_RotationSpeed = 0.75f;

	//camera
	const float fovAngle{ 45.0f };
	m_Camera.Initialize(m_AspectRatio, fovAngle, { 0.0f, 0.0f, 0.0f });
	m_Camera.CalculateViewMatrix();

	//lights
	const float		lightIntensity	{ 7.0f };
	const float		lightShininess	{ 25.0f };
	const Vector3	lighDirection	{ 0.577f, -0.577f, 0.577f };
	const ColorRGB	ambient			{ 0.025f, 0.025f, 0.025f };

	m_Light			= Utils::AddDirectionalLight(lighDirection, lightIntensity, colors::White, lightShininess, ambient);

	//textures
	m_pTexture		= Texture::LoadFromFile("Resources/vehicle_diffuse.png");
	m_pNormalMap	= Texture::LoadFromFile("Resources/vehicle_normal.png");
	m_pSpecular		= Texture::LoadFromFile("Resources/vehicle_specular.png");
	m_pGloss		= Texture::LoadFromFile("Resources/vehicle_gloss.png");

	//Mesh
	const std::string		vehicle{ "Resources/Vehicle.obj" };
	std::vector<Vertex>		vertices{};
	std::vector<uint32_t>	indices{};
	Utils::ParseOBJ(vehicle, vertices, indices);
	m_MeshVehicle.vertices	= vertices;
	m_MeshVehicle.indices	= indices;
	m_MeshVehicle.primitiveTopology = PrimitiveTopology::TriangleList;
}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);

	switch (m_Rotation)
	{
	case Rotation::Yes:
		m_yawn += pTimer->GetElapsed() * m_RotationSpeed;
		break;
	}
}

void Renderer::Render_W4_Vehicle()
{
	std::vector<Mesh> meshes
	{
		m_MeshVehicle
	};

	VertexTransformationFunction(meshes);
	for (const Mesh& mesh : meshes)
	{
		if (m_IsParallelOn)
		{
			const uint32_t amountOfTriangles = ((uint32_t)mesh.indices.size()) / 3;
			concurrency::parallel_for(static_cast<uint32_t>(0), amountOfTriangles, [=, this](int index)
			{
				Vertex_Out v0 = mesh.vertices_out[mesh.indices[3 * index]];
				Vertex_Out v1 = mesh.vertices_out[mesh.indices[3 * index + 1]];
				Vertex_Out v2 = mesh.vertices_out[mesh.indices[3 * index + 2]];

				RenderTriangleW4(v0, v1, v2);
			});

			////test this one out later
			//const uint32_t indicesSize = ((uint32_t)mesh.indices.size());
			//concurrency::parallel_for(static_cast<uint32_t>(0), indicesSize, [=, this](int index)
			//{
			//	Vertex_Out v0 = mesh.vertices_out[mesh.indices[index]];
			//	Vertex_Out v1 = mesh.vertices_out[mesh.indices[++index]];
			//	Vertex_Out v2 = mesh.vertices_out[mesh.indices[++index]];
			//	++index;

			//	RenderTriangleW4(v0, v1, v2);
			//});
		}
		else
		{
			int indicesSizeArray{ static_cast<int>(mesh.indices.size()) };
			for (int indicesIndex{}; indicesIndex < indicesSizeArray; ++indicesIndex)
			{
				Vertex_Out v0 = mesh.vertices_out[mesh.indices[indicesIndex]];
				Vertex_Out v1 = mesh.vertices_out[mesh.indices[++indicesIndex]];
				Vertex_Out v2 = mesh.vertices_out[mesh.indices[++indicesIndex]];


				RenderTriangleW4(v0, v1, v2);
			}
		}
	}
}

void Renderer::RenderTriangleW4(Vertex_Out& v0, Vertex_Out& v1, Vertex_Out& v2)
{
	//I do it here because there is no need to call the function right?
	//does it save any performance from not rendering it at all?

	//Disclaimer. FOR SOME REASON, IT DOES NOT WORK IF IT IS DONE BEFORE CALLING THE RENDERW4 FUNCTION
	//IDK WHY, it just doesnt work
	if ((FrustumCulling(v0)) || (FrustumCulling(v1)) || (FrustumCulling(v2)))
		return;

	TransformToRasterSpace(v0, v1, v2, m_Width, m_Height);

	std::vector<Vertex_Out> triangle
	{
		v0,
		v1,
		v2,
	};

	Vector3 minAABB{};
	Vector3 maxAABB{};
	CalculateAABB(triangle, minAABB, maxAABB, m_Width, m_Height);

	const Vector2 V0{ v0.position.x, v0.position.y };
	const Vector2 V1{ v1.position.x, v1.position.y };
	const Vector2 V2{ v2.position.x, v2.position.y };

	float area{ CalculateArea(V0, V1, V2) };

	if (area <= 0) // if area is negative, we are looking at the back of the triangle, hence, do not render
		return;

	ColorRGB finalColor{ colors::Black };

	for (int px{ static_cast<int>(minAABB.x) }; px < static_cast<int>(maxAABB.x); ++px)
	{
		for (int py{ static_cast<int>(minAABB.y) }; py < static_cast<int>(maxAABB.y); ++py)
		{
			switch (m_Mode)
			{
			case Mode::Hit:

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

				switch (m_Mode)
				{
				case Mode::Texture:
					if (depth > zInterpolated)
					{
						m_pDepthBufferPixels[currentPixel] = zInterpolated;
						const float wInterpolated{ CalculateInterpolatedW(triangle, w0, w1, w2) };

						//I dont make my vertex_out const because I change color
						const Vertex_Out pixelVertexPos{ CalculateInterpolatedVerTex_Out(triangle, w0, w1, w2, wInterpolated)};
						finalColor = PixelShading(pixelVertexPos);

						finalColor.MaxToOne();

						m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(finalColor.r * 255),
							static_cast<uint8_t>(finalColor.g * 255),
							static_cast<uint8_t>(finalColor.b * 255));
					}
					break;
				case Mode::Buffer:
					if (depth > zInterpolated)
					{
						m_pDepthBufferPixels[currentPixel] = zInterpolated;
						const float remapValue = Utils::Remap(m_pDepthBufferPixels[currentPixel]);

						m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(remapValue * 255),
							static_cast<uint8_t>(remapValue * 255),
							static_cast<uint8_t>(remapValue * 255));
					}
					break;
				}
			}
		}
	}
}

void Renderer::Render()
{
	std::fill_n(m_pDepthBufferPixels, m_Width * m_Height, FLT_MAX);
	SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));

	//@START
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);

	//Render_W1_RasterizationOnly();
	//Render_W1_ProjectionStage();
	//Render_W1_BarycentricCoordinates();
	//Render_W1_DepthBuffer();
	//Render_W2_Quad(); //render the tuktuk from here
	//Render_W3_TukTuk();

	Render_W4_Vehicle();

	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::VertexTransformationFunction(std::vector<Mesh>& meshes) const //W2 Version
{
	//calculate the normals with the world matrix only, never with the world view matrix
	//dont do a prepective divide onto the normals -> "only do it in on the position" -> hmmm what?
	//we only need the position
	//always normalize the interpolated result from the normal vectors


	Matrix worlViewProjectionMatrix;
	for (auto& mesh : meshes)
	{
		mesh.worldMatrix = Matrix::CreateRotationY(m_yawn) * Matrix::CreateTranslation({ 0.0f, 0.0f, 50.0f });
		worlViewProjectionMatrix = mesh.worldMatrix * m_Camera.worldViewProjectionMatrix;

		mesh.vertices_out.clear();
		for (auto& vertex : mesh.vertices)
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



//its not const on purpose
ColorRGB Renderer::PixelShading(const Vertex_Out& v) const
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
	const ColorRGB sampleTextureMap{ m_pTexture->Sample(v.uv) };
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
}

void Renderer::ToggleDepthBuffer()
{
	switch (m_Mode)
	{
	case Mode::Hit:
		m_Mode = Mode::Texture;
		std::cout << "You are now on Texture Viewing Mode" << '\n';
		break;
	case Mode::Texture:
		m_Mode = Mode::Buffer;
		std::cout << "You are now on Depth Buffer Viewing Mode" << '\n';
		break;
	case Mode::Buffer:
		m_Mode = Mode::Hit;
		std::cout << "You are now on Hit Box Viewing Mode" << '\n';
		break;
	}

}

void Renderer::ToggleRotation()
{
	switch (m_Rotation)
	{
	case Rotation::Yes:
		m_Rotation = Rotation::No;
		std::cout << "Rotation OFF" << '\n';
		break;
	case Rotation::No:
		m_Rotation = Rotation::Yes;
		std::cout << "Rotation ON" << '\n';
		break;
	}
}

void Renderer::ToggleShadingMode()
{
	switch (m_ShadingMode)
	{
	case ShadingMode::Combined:
		m_ShadingMode = ShadingMode::ObservedArea;
		std::cout << "You are now on Observed Area Viewing Mode" << '\n';
		break;
	case ShadingMode::ObservedArea:
		m_ShadingMode = ShadingMode::Diffuse;
		std::cout << "You are now on Diffuse Map Viewing Mode" << '\n';
		break;
	case ShadingMode::Diffuse:
		m_ShadingMode = ShadingMode::Specular;
		std::cout << "You are now on Specular Map Viewing Mode" << '\n';
		break;
	case ShadingMode::Specular:
		m_ShadingMode = ShadingMode::Combined;
		std::cout << "You are now on Combined Viewing Mode" << '\n';
		break;
		}
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}

//void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex>& vertices_out) const
//{
//	//Todo > W1 Projection Stage
//
//	vertices_out = vertices_in;
//	Matrix viewMatrix = m_Camera.viewMatrix;
//
//
//	for (auto& vertex : vertices_out)
//	{
//		//3 stepts required to transform a point from world space to NDC:
//
//		//transform to camera space // apply matrix
//		//1. transform all vertices from world space to camera space (view space)
//		vertex.position = viewMatrix.TransformPoint(vertex.position); 
//
//		//2.transform these vertices from the view space to the NDC(Normalized Device Coordinates) -> Prespective Divide
//		//The 3D point is now in the Projection Space
//		vertex.position.x = vertex.position.x / vertex.position.z;
//		vertex.position.y = vertex.position.y / vertex.position.z; //camera settings
//
//		//3. We need to consider the camera settings. Apply the aspect ratio and camera fov onto the vertex points.
//		vertex.position.x = (vertex.position.x) / (m_AspectRatio * m_Camera.fov);
//		vertex.position.y = (vertex.position.y) / m_Camera.fov;
//
//		//As a final step, we place every vertex point on the screen space before comparing with the actual pixels
//		vertex.position.x = (vertex.position.x + 1) / 2 * m_Width;
//		vertex.position.y = (1 - vertex.position.y) / 2 * m_Height;
//	}
//} // for vertices only
//
//void Renderer::VertexTransformationFunction(Mesh& mesh) const //Wx Version
//{
//	Matrix worldMatric = Matrix::CreateRotationY(m_yawn);
//	Matrix worlViewProjectionMatrix = worldMatric * m_Camera.worldViewProjectionMatrix;
//
//	for (auto& vertex : mesh.vertices)
//	{
//		//3 stepts required to transform a point from world space to NDC:
//
//		//Create a Vector4 from the original vertex Position adding 1 to the w value
//		Vector4 vertexPosition{ vertex.position.x,  vertex.position.y,  vertex.position.z, 1 };
//		//transform to camera space // apply matrix
//
//		//1. transform all vertices from world space to camera space (view space)
//		vertexPosition = worlViewProjectionMatrix.TransformPoint(vertexPosition);
//
//		//2. apply prespective devide
//		vertexPosition.x = vertexPosition.x / vertexPosition.w;
//		vertexPosition.y = vertexPosition.y / vertexPosition.w;
//		vertexPosition.z = vertexPosition.z / vertexPosition.w;
//
//		//3. As a final step, we place every vertex point on the screen space before comparing with the actual pixels
//		//vertexPosition.x = (vertexPosition.x + 1) / 2 * static_cast<float>(m_Width);
//		//vertexPosition.y = (1 - vertexPosition.y) / 2 * static_cast<float>(m_Height);
//
//		//4. create a vertex out from the previous calculations and using vertex color and uv
//		Vertex_Out newVertexOut{ vertexPosition, vertex.color, vertex.uv };
//		//5. place thme in the vector
//		mesh.vertices_out.emplace_back(newVertexOut);
//		////2.transform these vertices from the view space to the NDC(Normalized Device Coordinates) -> Prespective Divide
//		////The 3D point is now in the Projection Space
//		//vertex.position.x = vertex.position.x / vertex.position.z;
//		//vertex.position.y = vertex.position.y / vertex.position.z; //camera settings
//		//
//		////3. We need to consider the camera settings. Apply the aspect ratio and camera fov onto the vertex points.
//		//vertex.position.x = (vertex.position.x) / (m_AspectRatio * m_Camera.fov);
//		//vertex.position.y = (vertex.position.y) / m_Camera.fov;
//		//
//		////As a final step, we place every vertex point on the screen space before comparing with the actual pixels
//		//vertex.position.x = (vertex.position.x + 1) / 2 * m_Width;
//		//vertex.position.y = (1 - vertex.position.y) / 2 * m_Height;
//	}
//} // for a single mesh

void Renderer::InitializeTuktuk()
{
	m_Camera.Initialize(m_AspectRatio, 60.f, { 0.0f, 5.0f, -30.0f });
	m_Camera.CalculateViewMatrix();
	m_pTexture = Texture::LoadFromFile("Resources/tuktuk.png");

	std::vector<Vertex> vertices{};
	std::vector<uint32_t> indices{};
	std::string tuktuk{ "Resources/tuktuk.obj" };

	Utils::ParseOBJ(tuktuk, vertices, indices);

	m_MeshTuktuk.vertices = vertices;
	m_MeshTuktuk.indices = indices;
	m_MeshTuktuk.primitiveTopology = PrimitiveTopology::TriangleList;
}

void Renderer::Render_W3_TukTuk()
{
	std::vector<Mesh> meshes
	{
		m_MeshTuktuk
	};

	VertexTransformationFunction(meshes);
	for (const auto& mesh : meshes)
	{
		if (mesh.primitiveTopology == PrimitiveTopology::TriangleList)
		{
			int indicesSizeArray{ static_cast<int>(mesh.indices.size()) };
			for (int indicesIndex{}; indicesIndex < indicesSizeArray; ++indicesIndex)
			{
				Vertex_Out v0 = mesh.vertices_out[mesh.indices[indicesIndex]];
				Vertex_Out v1 = mesh.vertices_out[mesh.indices[++indicesIndex]];
				Vertex_Out v2 = mesh.vertices_out[mesh.indices[++indicesIndex]];

				RenderTriangle(v0, v1, v2);
			}
		}
	}
}

void Renderer::RenderTriangle(Vertex_Out& v0, Vertex_Out& v1, Vertex_Out& v2)
{




	if ((FrustumCulling(v0)) || (FrustumCulling(v1)) || (FrustumCulling(v2)))
		return;


	Utils::TransformToRasterSpace(v0, m_Width, m_Height);
	Utils::TransformToRasterSpace(v1, m_Width, m_Height);
	Utils::TransformToRasterSpace(v2, m_Width, m_Height);

	std::vector<Vertex_Out> triangle
	{
		v0,
		v1,
		v2,
	};

	Vector3 minAABB{};
	Vector3 maxAABB{};
	CalculateAABB(triangle, minAABB, maxAABB, m_Width, m_Height);

	const Vector2 V0{ v0.position.x, v0.position.y };
	const Vector2 V1{ v1.position.x, v1.position.y };
	const Vector2 V2{ v2.position.x, v2.position.y };



	float area{ Utils::CalculateArea(V0, V1, V2) };

	ColorRGB finalColor{ 0, 0, 0 };
	ColorRGB fromTexture{};

	for (int px{ static_cast<int>(minAABB.x) }; px < static_cast<int>(maxAABB.x); ++px)
	{
		for (int py{ static_cast<int>(minAABB.y) }; py < static_cast<int>(maxAABB.y); ++py)
		{
			Vector2 point{ static_cast<float>(px) + 0.5f,  static_cast<float>(py) + 0.5f };

			//float w0 = Vector2::Cross(v2 - v1, point - v1) / area;
			float w0{ Utils::CalculateWeights(V1, V2, point, area) };
			//float w1 = Vector2::Cross(v0 - v2, point - v2) / area;
			float w1{ Utils::CalculateWeights(V2, V0, point, area) };
			//float w2 = Vector2::Cross(v1 - v0, point - v0) / area;
			float w2{ Utils::CalculateWeights(V0, V1, point, area) };

			//const Vector3 pixelPosition = v0.position * w0 + v1.position * w1 + v2.position * w2;
			//float check = w0 + w1 + w2;

			const bool isInTriangle = w0 > 0 && w1 > 0 && w2 > 0;
			if (isInTriangle)
			{
				const float zInterpolated{ Utils::CalculateInterpolatedZ(triangle, w0, w1, w2) };
				const float wInterpolated{ Utils::CalculateInterpolatedW(triangle, w0, w1, w2) };

				const int currentPixel{ py * m_Width + px };
				const float depth = m_pDepthBufferPixels[currentPixel];

				if (zInterpolated < 0.0f || zInterpolated > 1.0f)
					continue;

				switch (m_Mode)
				{
				case Mode::Texture:
					if (depth > zInterpolated)
					{
						//float zInterpolated{ CalculateInterpolatedZ(triangle, w0, w1, w2) };
						const Vector2 interpolatedUV{ Utils::CalculateInterpolatedUV(triangle, w0, w1, w2, wInterpolated) };

						//m_pDepthBufferPixels[py * m_Width + px] = pixelPosition.z;
						m_pDepthBufferPixels[currentPixel] = zInterpolated;

						//finalColor = v0.color * w0 + v1.color * w1 + v2.color * w2;
						fromTexture = { m_pTexture->Sample(interpolatedUV) };

						fromTexture.MaxToOne();

						m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(fromTexture.r * 255),
							static_cast<uint8_t>(fromTexture.g * 255),
							static_cast<uint8_t>(fromTexture.b * 255));
					}
					break;
				case Mode::Buffer:
					if (depth > zInterpolated)
					{
						m_pDepthBufferPixels[currentPixel] = zInterpolated;
						float remapValue = Utils::Remap(m_pDepthBufferPixels[currentPixel]);

						m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
							static_cast<uint8_t>(remapValue * 255),
							static_cast<uint8_t>(remapValue * 255),
							static_cast<uint8_t>(remapValue * 255));
					}
					break;
				}
			}

		}
	}
}

#pragma region preivousRenders
//void Renderer::Render_W1_RasterizationOnly()
//{
//
//	//std::vector<Vertex> vertices_ndc
//	//{
//	//	{{0.0f, 0.5f, 1.0f}},
//	//	{{0.5f, -0.5f, 1.0f}},
//	//	{{-0.5f, -0.5f, 1.0f}}
//	//};
//
//	////Define Triangle - Vertices in NDC space
//	////Triangle triangle = { Vector3(0.0f, 0.5f, 1.0f), Vector3(0.5f, -0.5f, 1.0f), Vector3(-0.5f, -0.5f, 1.0f) };
//	////triangle.TransformToScreenSpace(static_cast<float>(m_Width), static_cast<float>(m_Height));
//
//
//	//for (auto& vertex : vertices_ndc)
//	//{
//	//	vertex.position.x = (vertex.position.x + 1) / 2 * m_Width;
//	//	vertex.position.y = (1 - vertex.position.y) / 2 * m_Height;
//	//}
//
//	//for (int px{}; px < m_Width; ++px)
//	//{
//	//	for (int py{}; py < m_Height; ++py)
//	//	{
//	//		ColorRGB finalColor{ 0, 0, 0 };
//
//	//		Vector3 point{ static_cast<float>(px),  static_cast<float>(py), 0 };
//
//	//		
//	//		if (PointInTriangle(vertices_ndc, point))
//	//		{
//	//			finalColor = { 1,1,1 };
//	//		}
//
//	//		//Update Color in Buffer
//	//		finalColor.MaxToOne();
//
//	//		m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
//	//			static_cast<uint8_t>(finalColor.r * 255),
//	//			static_cast<uint8_t>(finalColor.g * 255),
//	//			static_cast<uint8_t>(finalColor.b * 255));
//	//	}
//	//}
//}
//
//void Renderer::Render_W1_ProjectionStage()
//{
////	std::vector<Vertex> vertices_ndc
////	{ 
////		{{0.0f, 2.0f, 0.0f}},
////		{{1.0f, 0.0f, 0.0f}},
////		{{-1.0f, 0.0f, 0.0f}}
////	};
////
////
////	std::vector<Vertex> vertices{};
////	VertexTransformationFunction(vertices_ndc, vertices);
////
////	for (int px{}; px < m_Width; ++px)
////	{
////		for (int py{}; py < m_Height; ++py)
////		{
////			ColorRGB finalColor{ 0, 0, 0 };
////
////			Vector3 point{ static_cast<float>(px),  static_cast<float>(py), 0 };
////
////			if (PointInTriangle(vertices, point))
////			{
////				finalColor = { 1,1,1 };
////			}
////
////			//Update Color in Buffer
////			finalColor.MaxToOne();
////
////			m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
////				static_cast<uint8_t>(finalColor.r * 255),
////				static_cast<uint8_t>(finalColor.g * 255),
////				static_cast<uint8_t>(finalColor.b * 255));
////		}
////	}
//}
//
//void Renderer::Render_W1_BarycentricCoordinates()
//{
//	//std::vector<Vertex> vertices_ndc
//	//{
//	//	{{0.0f, 4.0f, 2.0f}, {1, 0, 0}},
//	//	{{3.0f, -2.0f, 2.0f}, {0, 1, 0}},
//	//	{{-3.0f, -2.0f, 2.0f}, {0, 0, 1}}
//	//};
//
//	//std::vector<Vertex> vertices{};
//	//VertexTransformationFunction(vertices_ndc, vertices);
//
//	////float area = triangleArea(vertices);
//
//
//	//for (int px{}; px < m_Width; ++px)
//	//{
//	//	for (int py{}; py < m_Height; ++py)
//	//	{
//	//		ColorRGB finalColor{ 0, 0, 0 };
//	//		
//	//		Vector3 point{ static_cast<float>(px),  static_cast<float>(py), 0 };
//
//	//		if (PointInTriangle(vertices, point))
//	//		{
//	//			float w0 = CalculateWeights(vertices[1].position, vertices[2].position, point, area); //v1 and v2
//	//			float w1 = CalculateWeights(vertices[2].position, vertices[0].position, point, area); //v2 and v0
//	//			float w2 = CalculateWeights(vertices[0].position, vertices[1].position, point, area); //v1 and v0
//	//			//float totalTriangleArea = (w0 + w1 + w2);
//	//			//assert((totalTriangleArea == 1) && (w0 >= 0) && (w1 >= 0) && (w2 >= 0) && "We have a problem in the triangle");
//
//
//	//			finalColor = { vertices[0].color * w0 + vertices[1].color * w1 + vertices[2].color * w2 };
//	//		}
//
//	//		//Update Color in Buffer
//	//		finalColor.MaxToOne();
//
//	//		m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
//	//			static_cast<uint8_t>(finalColor.r * 255),
//	//			static_cast<uint8_t>(finalColor.g * 255),
//	//			static_cast<uint8_t>(finalColor.b * 255));
//	//	}
//	//}
//}
//
//void Renderer::Render_W1_DepthBuffer()
//{
//	//std::vector<Vertex> vertices_ndc
//	//{
//	//	//triangle1
//	//	{{0.0f, 2.0f, 8.0f}, {1, 0, 0}},
//	//	{{1.5f, -1.0f, 0.0f}, {1, 0, 0}},
//	//	{{-1.5f, -1.0f, 0.0f}, {1, 0, 0}},
//
//	//	////triangle 2
//	//	{{0.0f, 4.0f, 2.0f}, {1, 0, 0}},
//	//	{{3.0f, -2.0f, 2.0f}, {0, 1, 0}},
//	//	{{-3.0f, -2.0f, 2.0f}, {0, 0, 1}}
//	//};
//
//	//std::vector<Vertex> vertices{};
//	//VertexTransformationFunction(vertices_ndc, vertices);
//
//
//	//int vectorSize{ static_cast<int>(vertices.size()) };
//	//for (int index{}; index < vectorSize; index += 3)
//	//{
//	//	std::vector<Vertex> triangle
//	//	{
//	//		vertices[index + 0],
//	//		vertices[index + 1],
//	//		vertices[index + 2],
//	//	};
//
//	//	Vector3 minAABB{};
//	//	Vector3 maxAABB{};
//	//	CalculateAABB(triangle, minAABB, maxAABB);
//
//	//
//	//	const Vector2 v0{ triangle[0].position.x, triangle[0].position.y };
//	//	const Vector2 v1{ triangle[1].position.x, triangle[1].position.y };
//	//	const Vector2 v2{ triangle[2].position.x, triangle[2].position.y };
//	//
//	//	float area{ CalculateArea(v0, v1, v2) };
//	//
//	//	for (int px{}; px < m_Width; ++px)
//	//	{
//	//		for (int py{}; py < m_Height; ++py)
//	//		{
//
//	//			ColorRGB finalColor{ 0, 0, 0 };
//	//
//	//			Vector2 point{ static_cast<float>(px)+0.5f,  static_cast<float>(py)+0.5f };
//
//	//			if (SlabTest(minAABB, maxAABB, point))
//	//			{
//	//				//float w0 = Vector2::Cross(v2 - v1, point - v1) / area;
//	//				float w0{ CalculateWeights(v1, v2, point, area) };
//	//				//float w1 = Vector2::Cross(v0 - v2, point - v2) / area;
//	//				float w1{ CalculateWeights(v2, v0, point, area) };
//	//				//float w2 = Vector2::Cross(v1 - v0, point - v0) / area;
//	//				float w2{ CalculateWeights(v0, v1, point, area) };
//
//	//				const Vector3 pixelPosition = vertices[index + 0].position * w0 + vertices[index + 1].position * w1 + vertices[index + 2].position * w2;
//
//	//				if (PointInTriangle(triangle, pixelPosition))
//	//				{
//	//					float depth = m_pDepthBufferPixels[py * m_Width + px];
//	//					if (depth > pixelPosition.z)
//	//					{
//	//						m_pDepthBufferPixels[py * m_Width + px] = pixelPosition.z;
//	//						finalColor = vertices[index].color * w0 + vertices[index + 1].color * w1 + vertices[index + 2].color * w2;
//	//						//Update Color in Buffer
//	//						finalColor.MaxToOne();
//
//	//						m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
//	//							static_cast<uint8_t>(finalColor.r * 255),
//	//							static_cast<uint8_t>(finalColor.g * 255),
//	//							static_cast<uint8_t>(finalColor.b * 255));
//
//	//					}
//	//				}
//	//			}
//	//		}
//	//	}
//	//}
//	
//}
//
//void Renderer::Render_W2_Quad()
//{
//	//DEFINE MESH
//
//	std::vector<Mesh> meshes
//	{
//		/*Mesh
//		{
//			{
//				Vertex{{ -3.0f,  3.0f, -2.0f},{},{ 0.0f, 0.0f}},
//				Vertex{{  0.0f,  3.0f, -2.0f},{},{ 0.5f, 0.0f}},
//				Vertex{{  3.0f,  3.0f, -2.0f},{},{ 1.0f, 0.0f}},
//				Vertex{{ -3.0f,  0.0f, -2.0f},{},{ 0.0f, 0.5f}},
//				Vertex{{  0.0f,  0.0f, -2.0f},{},{ 0.5f, 0.5f}},
//				Vertex{{  3.0f,  0.0f, -2.0f},{},{ 1.0f, 0.5f}},
//				Vertex{{ -3.0f, -3.0f, -2.0f},{},{ 0.0f, 1.0f}},
//				Vertex{{  0.0f, -3.0f, -2.0f},{},{ 0.5f, 1.0f}},
//				Vertex{{  3.0f, -3.0f, -2.0f},{},{ 1.0f, 1.0f}},
//			},
//			{
//				3,0,1,   1,4,3,   4,1,2,
//				2,5,4,   6,3,4,   4,7,6,
//				7,4,5,   5,8,7,
//			},
//			PrimitiveTopology::TriangleList,
//		},*/
//
//		//Mesh
//		//{
//		//	{
//		//		Vertex{{ -3.0f,  3.0f, -2.0f},{1, 1, 1},{ 0.0f, 0.0f}},
//		//		Vertex{{  0.0f,  3.0f, -2.0f},{1, 1, 1},{ 0.5f, 0.0f}},
//		//		Vertex{{  3.0f,  3.0f, -2.0f},{1, 1, 1},{ 1.0f, 0.0f}},
//		//		Vertex{{ -3.0f,  0.0f, -2.0f},{1, 1, 1},{ 0.0f, 0.5f}},
//		//		Vertex{{  0.0f,  0.0f, -2.0f},{1, 1, 1},{ 0.5f, 0.5f}},
//		//		Vertex{{  3.0f,  0.0f, -2.0f},{1, 1, 1},{ 1.0f, 0.5f}},
//		//		Vertex{{ -3.0f, -3.0f, -2.0f},{1, 1, 1},{ 0.0f, 1.0f}},
//		//		Vertex{{  0.0f, -3.0f, -2.0f},{1, 1, 1},{ 0.5f, 1.0f}},
//		//		Vertex{{  3.0f, -3.0f, -2.0f},{1, 1, 1},{ 1.0f, 1.0f}},
//		//	},
//		//	{
//		//		3,0,4,1,5,2,
//		//		2,6,
//		//		6,3,7,4,8,5
//		//	},
//		//	PrimitiveTopology::TriangleStrip,
//		//}, 
//		m_MeshTuktuk
//	};
//	VertexTransformationFunction(meshes);
//
//
//
//	for (const auto& mesh : meshes)
//	{
//		if (mesh.primitiveTopology == PrimitiveTopology::TriangleStrip)
//		{
//			int indicesSizeArray{ static_cast<int>(mesh.indices.size()) - 2 };
//			for (int indicesIndex{}; indicesIndex < indicesSizeArray; ++indicesIndex)
//			{
//
//				if (indicesIndex % 2 == 0)
//				{
//					Vertex_Out  v0 = mesh.vertices_out[mesh.indices[indicesIndex]];
//					Vertex_Out  v1 = mesh.vertices_out[mesh.indices[1 + indicesIndex]];
//					Vertex_Out  v2 = mesh.vertices_out[mesh.indices[2 + indicesIndex]];
//
//					RenderTriangle(v0, v1, v2);
//				}
//				else
//				{
//					Vertex_Out  v0 = mesh.vertices_out[mesh.indices[indicesIndex]];
//					Vertex_Out  v1 = mesh.vertices_out[mesh.indices[2 + indicesIndex]];
//					Vertex_Out  v2 = mesh.vertices_out[mesh.indices[1 + indicesIndex]];
//
//					RenderTriangle(v0, v1, v2);
//				}
//			}
//		}
//		if (mesh.primitiveTopology == PrimitiveTopology::TriangleList)
//		{
//			int indicesSizeArray{ static_cast<int>(mesh.indices.size()) };
//			for (int indicesIndex{}; indicesIndex < indicesSizeArray; ++indicesIndex)
//			{
//				Vertex_Out v0 = mesh.vertices_out[mesh.indices[indicesIndex]];
//				Vertex_Out v1 = mesh.vertices_out[mesh.indices[++indicesIndex]];
//				Vertex_Out v2 = mesh.vertices_out[mesh.indices[++indicesIndex]];
//
//				RenderTriangle(v0, v1, v2);
//			}
//		}
//	}
//}
//
#pragma endregion preivousRenders

