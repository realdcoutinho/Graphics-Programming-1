#pragma once
#include <cassert>

#include "Math.h"
#include "vector"
#include <iostream>

namespace dae
{
#pragma region GEOMETRY
	struct Sphere
	{
		Vector3 origin{};
		float radius{};

		unsigned char materialIndex{ 0 };
	};

	struct Plane
	{
		Vector3 origin{};
		Vector3 normal{};

		unsigned char materialIndex{ 0 };
	};

	enum class TriangleCullMode
	{
		FrontFaceCulling,
		BackFaceCulling,
		NoCulling
	};

	struct Triangle
	{
		Triangle() = default;
		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Vector3& _normal):
			v0{_v0}, v1{_v1}, v2{_v2}, normal{_normal.Normalized()}
		{
			//positions.push_back(v0);
			//positions.push_back(v1);
			//positions.push_back(v2);
			//UpdateAABB();
		}

		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2) :
			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }
		{
			const Vector3 edgeV0V1 = v1 - v0;
			const Vector3 edgeV0V2 = v2 - v0;
			normal = Vector3::Cross(edgeV0V1, edgeV0V2).Normalized();
		}

		Vector3 v0{};
		Vector3 v1{};
		Vector3 v2{};

		Vector3 normal{};

		TriangleCullMode cullMode{};
		unsigned char materialIndex{};



		Vector3 minAABB{};
		Vector3 maxAABB{};

		std::vector<Vector3> positions{};



		//void UpdateAABB()
		//{
		//	if (positions.size() > 0)
		//	{
		//		minAABB = positions[0];
		//		maxAABB = positions[0];
		//		for (auto& pos : positions)
		//		{
		//			minAABB = Vector3::Min(pos, minAABB);
		//			maxAABB = Vector3::Min(pos, maxAABB);
		//		}
		//	}
		//}


	};

	struct TriangleMesh
	{
		TriangleMesh() = default;
		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, TriangleCullMode _cullMode):
		positions(_positions), indices(_indices), cullMode(_cullMode)
		{
			//Calculate Normals
			CalculateNormals();

			//Update Transforms
			UpdateTransforms();
		}

		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, const std::vector<Vector3>& _normals, TriangleCullMode _cullMode) :
			positions(_positions), indices(_indices), normals(_normals), cullMode(_cullMode)
		{
			UpdateTransforms();
		}

		std::vector<Vector3> positions{};
		std::vector<Vector3> normals{};
		std::vector<int> indices{};
		unsigned char materialIndex{};

		TriangleCullMode cullMode{TriangleCullMode::BackFaceCulling};

		Matrix rotationTransform{};
		Matrix translationTransform{};
		Matrix scaleTransform{};
		Matrix finalTransform{};

		std::vector<Vector3> transformedPositions{};
		std::vector<Vector3> transformedNormals{};

		//mine
		Vector3 minAABB{};
		Vector3 maxAABB{};

		Vector3 transformedMinAABB{};
		Vector3 transformedMaxAABB{};


		void CalculateAABB()
		{
			minAABB = positions[0];
			maxAABB = positions[0];
			for (const Vector3& pos : positions)
			{
				//get min
				if (pos.x < minAABB.x)
					minAABB.x = pos.x;
				if (pos.y < minAABB.y)
					minAABB.y = pos.y;
				if (pos.z < minAABB.z)
					minAABB.z = pos.z;

				//get max
				if (pos.x > maxAABB.x)
					maxAABB.x = pos.x;
				if (pos.y > maxAABB.y)
					maxAABB.y = pos.y;
				if (pos.z > maxAABB.z)
					maxAABB.z = pos.z;
			}
		}

		void UpdateAABB()
		{
			if (positions.size() > 0)
			{
				minAABB = positions[0];
				maxAABB = positions[0];
				for (auto& pos : positions)
				{
					minAABB = Vector3::Min(pos, minAABB);
					maxAABB = Vector3::Min(pos, maxAABB);
				}
			}
		}

		void UpdateTransformedAABB(const Matrix& finalTransform)
		{
			//AABB update: be careful-> transform the 8 vertices of the aabb
			//and calculate new min and max
			Vector3 tMinAABB = finalTransform.TransformPoint(minAABB);
			Vector3 tMaxAABB = tMinAABB;

			//(xmax, ymin, zmin)
			Vector3 tAABB = finalTransform.TransformPoint(maxAABB.x, minAABB.y, minAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			//(xmax, ymin, zmax)
			tAABB = finalTransform.TransformPoint(maxAABB.x, minAABB.y, maxAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			//(xmin, ymin, zmax)
			tAABB = finalTransform.TransformPoint(minAABB.x, minAABB.y, maxAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			//(xmin, ymax, zmin)
			tAABB = finalTransform.TransformPoint(minAABB.x, maxAABB.y, minAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			//(xmax, ymax, zmin)
			tAABB = finalTransform.TransformPoint(maxAABB.x, maxAABB.y, minAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			//(xmax, ymax, zmax)
			tAABB = finalTransform.TransformPoint(maxAABB);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			//(xmin, ymax, zmax)
			tAABB = finalTransform.TransformPoint(minAABB.x, maxAABB.y, minAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			transformedMinAABB = tMinAABB;
			transformedMaxAABB = tMaxAABB;
		}


		void Translate(const Vector3& translation)
		{
			translationTransform = Matrix::CreateTranslation(translation);
		}

		void RotateY(float yaw)
		{
			rotationTransform = Matrix::CreateRotationY(yaw);
		}
		void RotateX(float pitch)
		{
			rotationTransform = Matrix::CreateRotationX(pitch);
		}
		void RotateZ(float rool)
		{
			rotationTransform = Matrix::CreateRotationZ(rool);
		}
		void Rotate(float pitch, float yaw, float roll)
		{
			rotationTransform = Matrix::CreateRotation(pitch, yaw, roll);
		}

		void Scale(const Vector3& scale)
		{
			scaleTransform = Matrix::CreateScale(scale);
		}

		void AppendTriangle(const Triangle& triangle, bool ignoreTransformUpdate = false)
		{
			int startIndex = static_cast<int>(positions.size());

			positions.push_back(triangle.v0);
			positions.push_back(triangle.v1);
			positions.push_back(triangle.v2);

			indices.push_back(startIndex);
			indices.push_back(++startIndex);
			indices.push_back(++startIndex);

			normals.push_back(triangle.normal);

			//Not ideal, but making sure all vertices are updated
			if(!ignoreTransformUpdate)
				UpdateTransforms();
		}

		void CalculateNormals()
		{
			const int sizeIndices{ static_cast<int>(indices.size()) };

			for (int index{}; index < sizeIndices; ++index)
			{
				
				Vector3 v0{ positions[indices[index]] };
				Vector3 v1{ positions[indices[++index]] };
				Vector3 v2{ positions[indices[++index]] };

				Vector3 edgeA{ v1 - v0 };
				Vector3 edgeB{ v2 - v1 };
				//Vector3 edgeC{ v0 - v2 };

				Vector3 aCrossB{ Vector3::Cross(edgeA, edgeB) };
				//Vector3 bCrossC{ Vector3::Cross(edgeB, edgeC) };
				//Vector3 cCrossA{ Vector3::Cross(edgeC, edgeA) };

				Vector3 normalOne{ aCrossB.Normalized() };
				//Vector3 normalTwo{ bCrossC.Normalized() };
				//Vector3 normalThree{ cCrossA.Normalized() };

				normals.push_back(normalOne);
			}
		}

		void UpdateTransforms()
		{
			//Calculate Final Transform 
			const auto finalTransform = scaleTransform * rotationTransform * translationTransform;

			//Transform Positions (positions > transformedPositions)
			//...
			transformedPositions.clear();
			transformedPositions.reserve(positions.size());
			for (Vector3& pos : positions)
			{
				Vector3 transPos{ finalTransform.TransformPoint(pos) };
				transformedPositions.emplace_back(transPos);
			}

			//Transform Normals (normals > transformedNormals)
			//...
			transformedNormals.clear();
			transformedNormals.reserve(normals.size());
			for (Vector3& nom : normals)
			{
				Vector3 transNom{ finalTransform.TransformVector(nom) };
				transformedNormals.emplace_back(transNom);
			}

			for (auto& p : positions)
			{
				transformedPositions.emplace_back(finalTransform.TransformPoint(p));
			}

			//Update AABB
			UpdateTransformedAABB(finalTransform);
		}


	};
#pragma endregion
#pragma region LIGHT
	enum class LightType
	{
		Point,
		Directional
	};

	struct Light
	{
		Vector3 origin{};
		Vector3 direction{};
		ColorRGB color{};
		float intensity{};

		LightType type{};
	};
#pragma endregion
#pragma region MISC
	struct Ray
	{
		Vector3 origin{};
		Vector3 direction{};

		float min{ 0.0001f };
		float max{ FLT_MAX };
	};

	struct HitRecord
	{
		Vector3 origin{};
		Vector3 normal{};
		float t = FLT_MAX;

		bool didHit{ false };
		unsigned char materialIndex{ 0 };
	};
#pragma endregion
}




//#define Sphere_Geometric
//#define Sphere_Quadratic
//#define Triangle_MollerTrumbore
//#define Triangle_Geometric
//
//
//#if defined(Sphere_Quadratic)
//return HitTest_Sphere_Quadratic(sphere, ray, temp, true);
//#elif defined(Sphere_Geometric)
//return HitTest_Sphere_Geometric(sphere, ray, temp, true);
//#endif