#include "Scene.h"
#include "Utils.h"
#include "Material.h"
#include <iostream>


namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene():
		m_Materials({ new Material_SolidColor({1,0,0})})
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for(auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		//todo W1
		const int sizeVectorSphere{ static_cast<int>(m_SphereGeometries.size()) };
		const int sizeVectorPlanes{ static_cast<int>(m_PlaneGeometries.size()) };
		const int sizeVectorTriangles{ static_cast<int>(m_Triangles.size()) };
		const int sizeVectorMesh{ static_cast<int>(m_TriangleMeshGeometries.size()) };

		for (int i{}; i < sizeVectorSphere; ++i)
		{
			HitRecord hitInfo{};
			GeometryUtils::HitTest_Sphere(m_SphereGeometries[i], ray, hitInfo);
			if (hitInfo.t < closestHit.t) // ask about it
			{
				closestHit = hitInfo;
			}
		}

		for (int i{}; i < sizeVectorPlanes; ++i)
		{
			HitRecord hitInfo{};
			GeometryUtils::HitTest_Plane(m_PlaneGeometries[i], ray, hitInfo);
			if (hitInfo.t < closestHit.t) // ask about it
			{
				closestHit = hitInfo;
			}
		}
		for (int i{}; i < sizeVectorTriangles; ++i)
		{
			HitRecord hitInfo{};
			GeometryUtils::HitTest_Triangle(m_Triangles[i], ray, hitInfo);
			if (hitInfo.t < closestHit.t) // ask about it
			{
				closestHit = hitInfo;
			}
		}
		for (int i{}; i < sizeVectorMesh; ++i)
		{
			HitRecord hitInfo{};
			GeometryUtils::HitTest_TriangleMesh(m_TriangleMeshGeometries[i], ray, hitInfo, false, closestHit.didHit);
			if (hitInfo.t < closestHit.t) // ask about it
			{
				closestHit = hitInfo;
			}




		}
	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		const int sizeVectorSphere{ static_cast<int>(m_SphereGeometries.size()) };
		for (int i{}; i < sizeVectorSphere; ++i)
		{
			if (GeometryUtils::HitTest_Sphere(m_SphereGeometries[i], ray)) 
				return true;
		}

		const int sizeVectorPlane{static_cast<int>(m_PlaneGeometries.size())};
		for (int i{}; i < sizeVectorPlane; ++i)
		{
			if (GeometryUtils::HitTest_Plane(m_PlaneGeometries[i], ray)) 
				return true;
		}
		const int sizeVectortTriangles{ static_cast<int>(m_Triangles.size()) };
		for (int i{}; i < sizeVectortTriangles; ++i)
		{
			if (GeometryUtils::HitTest_Triangle(m_Triangles[i], ray))
				return true;
		}
		const int sizeVectorMesh{ static_cast<int>(m_TriangleMeshGeometries.size()) };
		for (int i{}; i < sizeVectorMesh; ++i)
		{
			if (GeometryUtils::HitTest_TriangleMesh(m_TriangleMeshGeometries[i], ray))
				return true;
		}
		return false;
	}

	#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W1::Initialize()
	{
				//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({ -25.f, 0.f, 100.f }, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Plane
		AddPlane({ -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);

	#pragma region w1 todo 1

		float dotResult{};
		dotResult = Vector3::Dot(Vector3::UnitX, Vector3::UnitX); //1
		dotResult = Vector3::Dot(Vector3::UnitX, -Vector3::UnitX); //-1
		dotResult = Vector3::Dot(Vector3::UnitX, Vector3::UnitY); //-1

		Vector3 crossResult{};
		crossResult = Vector3::Cross(Vector3::UnitX, Vector3::UnitY);
		crossResult = Vector3::Cross(Vector3::UnitY, Vector3::UnitZ);

	#pragma endregion w1 todo 1

		


	}
#pragma endregion

#pragma region SCENE W2
	void Scene_W2::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = { 45.f };

		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });


		//Plane
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matId_Solid_Green);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matId_Solid_Green);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matId_Solid_Magenta);


		//Spheres
		AddSphere({ -1.75, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 1.75, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);

		//Light
		AddPointLight({ 0.f, 5.f, -5.f }, 70.f, colors::White);
	}

#pragma endregion

#pragma region SCENE W3
	void Scene_W3::Initialize()
	{
		m_Camera.origin = { 0.0f, 3.0f, -9.0f };
		m_Camera.fovAngle = 45.0f;

		const auto matCT_GrayRoughMetal		= AddMaterial(new Material_CookTorrence({ 0.972f,0.960f, 0.915f }, 1.0f, 1.0f));
		const auto matCT_GrayMediumMetal	= AddMaterial(new Material_CookTorrence({ 0.972f,0.960f, 0.915f }, 1.0f, 0.6f));
		const auto matCT_GraySmoothMetal	= AddMaterial(new Material_CookTorrence({ 0.972f,0.960f, 0.915f }, 1.0f, 0.1f));
		const auto matCT_GrayRoughPlastic	= AddMaterial(new Material_CookTorrence({ 0.750f,0.750f, 0.759f }, 0.0f, 1.0f));
		const auto matCT_GrayMediumPlastic	= AddMaterial(new Material_CookTorrence({ 0.750f,0.750f, 0.759f }, 0.0f, 0.6f));
		const auto matCT_GraySmoothPlastic	= AddMaterial(new Material_CookTorrence({ 0.750f,0.750f, 0.759f }, 0.0f, 0.1f));

		const auto matLamber_GrayBlue		= AddMaterial(new Material_Lambert({ 0.49f,0.57f, 0.57f }, 1.0f));

		//Plane
		AddPlane(Vector3{ 0.0f, 0.0f, 10.0f }, Vector3{ 0.0f, 0.0f, -1.0f }, matLamber_GrayBlue); //BACK
		AddPlane(Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.0f, 1.0f, 0.0f }, matLamber_GrayBlue);  //BOTTOM
		AddPlane(Vector3{ 0.0f, 10.0f, 0.0f }, Vector3{ 0.0f, -1.0f, 0.0f }, matLamber_GrayBlue); //TOP
		AddPlane(Vector3{ 5.0f, 0.0f, 10.0f }, Vector3{ -1.0f, 0.0f, 0.0f }, matLamber_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.0f, 0.0f, 10.0f }, Vector3{ 1.0f, 0.0f, 0.0f }, matLamber_GrayBlue); //LEFT

		//Spheres
		AddSphere(Vector3{ -1.75f, 1.0f, 0.0f }, 0.75f, matCT_GrayRoughMetal);
		AddSphere(Vector3{ 0.0f, 1.0f, 0.0f }, 0.75f, matCT_GrayMediumMetal);
		AddSphere(Vector3{ 1.75f, 1.0f, 0.0f }, 0.75f, matCT_GraySmoothMetal);
		AddSphere(Vector3{ -1.75f, 3.0f, 0.0f }, 0.75f, matCT_GrayRoughPlastic);
		AddSphere(Vector3{ 0.0f, 3.0f, 0.0f }, 0.75f, matCT_GrayMediumPlastic);
		AddSphere(Vector3{ 1.75f, 3.0f, 0.0f }, 0.75f, matCT_GraySmoothPlastic);

		//Light
		AddPointLight(Vector3{ 0.0f, 5.0f, 5.0f }, 50.f, ColorRGB{ 1.0f, 0.61f, 0.45f }); //BackLight
		AddPointLight(Vector3{ -2.5f, 5.0f, -5.0f }, 70.f, ColorRGB{ 1.0f, 0.8f, 0.45f }); //FrontLight Left
		AddPointLight(Vector3{ 2.5f, 2.5f, -5.0f }, 50.f, ColorRGB{ 0.34f, 0.47f, 0.68f });
	}

#pragma endregion

#pragma region SCENE W3 TestScene

	void Scene_W3_TestScene::Initialize()
	{
		m_Camera.origin = { 0.0f, 1.0f, -5.0f };
		m_Camera.fovAngle = 45.0f;

		//default: Material id0 >> SolidColor Material (RED)
		//constexpr unsigned char matId_Solid_Red = 0;
		//const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });
		//const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });

		const auto matLambet_Red	= AddMaterial(new Material_Lambert(colors::Red, 1.0f));
		//const auto matLambet_Blue	= AddMaterial(new Material_Lambert(colors::Blue, 1.0f));
		const auto matLambet_Yellow = AddMaterial(new Material_Lambert(colors::Yellow, 1.0f));

		const auto matPhong_Blue = AddMaterial(new Material_LambertPhong(colors::Blue, 1.f, 1.f, 60.0f));

		const auto matCookTorrence_Yellow = AddMaterial(new Material_CookTorrence({0.04f, 0.04f, 0.04f}, 1.0f, 0.1f));


		//Spheres
		AddSphere({ -0.75, 1.f, 0.0f }, 1.0f, matCookTorrence_Yellow);
		AddSphere({ 0.75, 1.f, 0.0f }, 1.0f, matPhong_Blue);

		//Plane
		AddPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, matLambet_Red);

		//Light
		AddPointLight({ 0.0f, 5.0f, 5.0f }, 25.0f, colors::White);
		AddPointLight({ 0.0f, 2.5f, -5.0f }, 25.0f, colors::White);
	}
#pragma endregion

#pragma region SCENE W4 TestScene

	void Scene_W4_TestScene::Initialize()
	{
		m_Camera.origin = { 0.0f, 1.0f, -5.0f };

		m_Camera.fovAngle = 45.0f;

		//Materials
		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({0.49f, 0.57f, 0.57f}, 1.0f));
		const auto matLamber_White = AddMaterial(new Material_Lambert(colors::White, 1.0f));
	
		//Planes
	//Plane
		AddPlane(Vector3{ 0.0f, 0.0f, 10.0f }, Vector3{ 0.0f, 0.0f, -1.0f }, matLambert_GrayBlue); //BACK
		AddPlane(Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.0f, 1.0f, 0.0f }, matLambert_GrayBlue);  //BOTTOM
		AddPlane(Vector3{ 0.0f, 10.0f, 0.0f }, Vector3{ 0.0f, -1.0f, 0.0f }, matLambert_GrayBlue); //TOP
		AddPlane(Vector3{ 5.0f, 0.0f, 10.0f }, Vector3{ -1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.0f, 0.0f, 10.0f }, Vector3{ 1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //LEFT

		////Triangle (Temp)
		//auto triangle = Triangle{ {-0.75f, 0.5f, 0.0f}, {-0.75f, 2.0f, 0.0f}, {0.75f, 0.5f, 0.0f } };

		//triangle.cullMode = TriangleCullMode::NoCulling;
		//triangle.materialIndex = matLamber_White;

		//m_Triangles.emplace_back(triangle);

		//const auto triangleMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLamber_White);
		//triangleMesh->positions = { 
		//	{-0.75f, -1.0f, 0.0f}, 
		//	{-0.75f, 1.0f, 0.0f}, 
		//	{0.75f, 1.0f, 1.0f}, 
		//	{0.75, -1.0f, 0.0f} };
		//triangleMesh->indices = {
		//	0, 1, 2, //Triangle1
		//	0, 2, 3 //Triangle2
		//};

		//triangleMesh->CalculateNormals();

		//triangleMesh->Translate({ 0.0f, 1.5f, 0.0f });
		//triangleMesh->RotateY(45);

		//triangleMesh->UpdateTransforms();



		pMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLamber_White);

		pMesh->positions = {
			{-0.75f, -1.0f, 0.0f},
			{-0.75f, 1.0f, 0.0f},
			{0.75f, 1.0f, 1.0f},
			{0.75, -1.0f, 0.0f} };
		pMesh->indices = {
			0, 1, 2, //Triangle1
			0, 2, 3 //Triangle2
		};

		pMesh->CalculateNormals();

		pMesh->Translate({ 0.0f, 1.5f, 0.0f });

		pMesh->UpdateTransforms();


		//pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLamber_White);
		//Utils::ParseOBJ("Resources/simple_cube.obj", pMesh->positions, pMesh->normals, pMesh->indices);

		//pMesh->Scale({ 0.7f, 0.7f, 0.7f });
		//pMesh->Translate({ 0.0f, 1.0f, 0.0f });

		//pMesh->UpdateTransforms();




		//Lights
		AddPointLight({ 0.0f, 5.0f, 5.0f }, 50.0f, { 1.0f, 0.61f, 0.45f });
		AddPointLight({ -2.5f, 5.0f, -5.0f }, 70.0f, { 1.0f, 0.8f, 0.45f });
		AddPointLight({ 2.5f, 5.0f, -5.0f }, 50.0f, { 0.34f, 0.47f, 0.68f });
	
	
	}

	void Scene_W4_TestScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		pMesh->RotateY(PI_DIV_2 * pTimer->GetTotal());
		pMesh->UpdateTransforms();
	}


#pragma region SCENE W4 

	void Scene_W4_ReferenceScene::Initialize()
	{
		sceneName = "Rederence Scene";
		m_Camera.origin = { 0.0f, 3.0f, -9.0f };
		m_Camera.fovAngle = 45.0f;

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ 0.972f,0.960f, 0.915f }, 1.0f, 1.0f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ 0.972f,0.960f, 0.915f }, 1.0f, 0.6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ 0.972f,0.960f, 0.915f }, 1.0f, 0.1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ 0.750f,0.750f, 0.759f }, 0.0f, 1.0f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ 0.750f,0.750f, 0.759f }, 0.0f, 0.6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ 0.750f,0.750f, 0.759f }, 0.0f, 0.1f));

		const auto matLamber_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f,0.57f, 0.57f }, 1.0f));
		const auto matLamber_White = AddMaterial(new Material_Lambert(colors::White, 1.0f));

		//Plane
		AddPlane(Vector3{ 0.0f, 0.0f, 10.0f }, Vector3{ 0.0f, 0.0f, -1.0f }, matLamber_GrayBlue); //BACK
		AddPlane(Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.0f, 1.0f, 0.0f }, matLamber_GrayBlue);  //BOTTOM
		AddPlane(Vector3{ 0.0f, 10.0f, 0.0f }, Vector3{ 0.0f, -1.0f, 0.0f }, matLamber_GrayBlue); //TOP
		AddPlane(Vector3{ 5.0f, 0.0f, 10.0f }, Vector3{ -1.0f, 0.0f, 0.0f }, matLamber_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.0f, 0.0f, 0.0f }, Vector3{ 1.0f, 0.0f, 0.0f }, matLamber_GrayBlue); //LEFT

		AddSphere(Vector3{ -1.75f, 1.0f, 0.0f }, 0.75f, matCT_GrayRoughMetal);
		AddSphere(Vector3{ 0.0f, 1.0f, 0.0f }, 0.75f, matCT_GrayMediumMetal);
		AddSphere(Vector3{ 1.75f, 1.0f, 0.0f }, 0.75f, matCT_GraySmoothMetal);
		AddSphere(Vector3{ -1.75f, 3.0f, 0.0f }, 0.75f, matCT_GrayRoughPlastic);
		AddSphere(Vector3{ 0.0f, 3.0f, 0.0f }, 0.75f, matCT_GrayMediumPlastic);
		AddSphere(Vector3{ 1.75f, 3.0f, 0.0f }, 0.75f, matCT_GraySmoothPlastic);


		const Triangle baseTriangle = { Vector3(-0.75f, 1.5f, 0.0f), Vector3(0.75, 0.0f, 0.0f), Vector3(-0.75f, 0.0f, 0.0f) };

		m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLamber_White);
		m_Meshes[0]->AppendTriangle(baseTriangle, true);
		m_Meshes[0]->Translate({ -1.75f, 4.5f, 0.0f });
		m_Meshes[0]->UpdateTransforms();

		m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLamber_White);
		m_Meshes[1]->AppendTriangle(baseTriangle, true);
		m_Meshes[1]->Translate({ 0.0f, 4.5f, 0.0f });
		m_Meshes[1]->UpdateTransforms();

		m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLamber_White);
		m_Meshes[2]->AppendTriangle(baseTriangle, true);
		m_Meshes[2]->Translate({ 1.75f, 4.5f, 0.0f });
		m_Meshes[2]->UpdateTransforms();

		AddPointLight({ 0.0f, 5.0f, 5.0f }, 50.0f, { 1.0f, 0.61f, 0.45f });
		AddPointLight({ -2.5f, 5.0f, -5.0f }, 70.0f, { 1.0f, 0.8f, 0.45f });
		AddPointLight({ 2.5f, 5.0f, -5.0f }, 50.0f, { 0.34f, 0.47f, 0.68f });


	}

	void Scene_W4_ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		const auto yawAngle = (cos(pTimer->GetTotal()) + 1.0f) / 2.0f * PI_2;
		for (const auto m : m_Meshes)
		{
			m->RotateY(yawAngle);
			m->UpdateTransforms();
		}
	}

	void Scene_W4_Bunny::Initialize()
	{
		m_Camera.origin = { 0.0f, 1.0f, -5.0f };

		m_Camera.fovAngle = 45.0f;

		//Materials
		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.0f));
		const auto matLamber_White = AddMaterial(new Material_Lambert(colors::White, 1.0f));

		//Planes
		AddPlane(Vector3{ 0.0f, 0.0f, 10.0f }, Vector3{ 0.0f, 0.0f, -1.0f }, matLambert_GrayBlue); //BACK
		AddPlane(Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.0f, 1.0f, 0.0f }, matLambert_GrayBlue);  //BOTTOM
		AddPlane(Vector3{ 0.0f, 10.0f, 0.0f }, Vector3{ 0.0f, -1.0f, 0.0f }, matLambert_GrayBlue); //TOP
		AddPlane(Vector3{ 5.0f, 0.0f, 10.0f }, Vector3{ -1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //RIGHT
		AddPlane(Vector3{ -5.0f, 0.0f, 10.0f }, Vector3{ 1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //LEFT



		pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLamber_White);
		Utils::ParseOBJ("Resources/lowpoly_bunny.obj", 
			pMesh->positions, 
			pMesh->normals, 
			pMesh->indices);

		pMesh->Scale({ 2.0f, 2.0f, 2.0f });
		pMesh->UpdateAABB();



		//pMesh->RotateY(180);

		pMesh->UpdateTransforms();

		//Lights
		AddPointLight({ 0.0f, 5.0f, 5.0f }, 50.0f, { 1.0f, 0.61f, 0.45f });
		AddPointLight({ -2.5f, 5.0f, -5.0f }, 70.0f, { 1.0f, 0.8f, 0.45f });
		AddPointLight({ 2.5f, 5.0f, -5.0f }, 50.0f, { 0.34f, 0.47f, 0.68f });


	}

}

