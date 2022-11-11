#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"
#include "Scene.h"

//#define Sphere_Quadratic
#define Sphere_Geometric
#define Triangle_MollerTrumbore
//#define Triangle_Geometric
//#define Sphere_SlabTest


extern bool m_SlabTestOn;

namespace dae
{
	namespace GeometryUtils
	{



#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool SpehereSlabTest(const Sphere& sphere, const Ray& ray)
		{
			float tx1 = (sphere.minAABB.x - ray.origin.x) / ray.direction.x;
			float tx2 = (sphere.maxAABB.x - ray.origin.x) / ray.direction.x;

			float tMin = std::min(tx1, tx2);
			float tMax = std::max(tx1, tx2);

			float ty1 = (sphere.minAABB.y - ray.origin.y) / ray.direction.y;
			float ty2 = (sphere.maxAABB.y - ray.origin.y) / ray.direction.y;

			tMin = std::max(tMin, std::min(ty1, ty2));
			tMax = std::min(tMax, std::max(ty1, ty2));

			float tz1 = (sphere.minAABB.z - ray.origin.z) / ray.direction.z;
			float tz2 = (sphere.maxAABB.z - ray.origin.z) / ray.direction.z;

			tMin = std::max(tMin, std::min(tz1, tz2));
			tMax = std::min(tMax, std::max(tz1, tz2));

			return tMax > 0 && tMax >= tMin;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{


			#if defined(Sphere_Quadratic) // done in class
			//Quadratic equation A B and C
			const float A{ Vector3::Dot(ray.direction, ray.direction) };
			const float B{ Vector3::Dot(2 * ray.direction, ray.origin - sphere.origin) };
			const float C{ Vector3::Dot(ray.origin - sphere.origin,  ray.origin - sphere.origin) - powf(sphere.radius, 2) };

			const float tMax{ ray.max };
			const float tMin{ ray.min };
			const float discriminant{ powf(B, 2) - (4 * A * C) };

			if (discriminant < 0)
			{
				//ray does not intersect
				hitRecord.didHit = false;
				return false;
			}
			if (discriminant == 0)
			{
				//ray touches the sphere in one point
				return false;
			}
			if (discriminant >= 0)
			{
				float t{ (-B - sqrt(discriminant)) / 2 * A };
				if (t >= tMin && t <= tMax)
				{
					//ray touches intersects the sphere in two points

					Vector3 p{ ray.origin + t * ray.direction };

					Vector3 vectorSphereCenterToP{ p.x - sphere.origin.x, p.y - sphere.origin.y, p.z - sphere.origin.z };

					hitRecord.didHit = true;
					hitRecord.t = t;
					hitRecord.origin = p;
					hitRecord.materialIndex = sphere.materialIndex;
					hitRecord.normal = vectorSphereCenterToP.Normalized();
					return hitRecord.didHit;
				}
			}
			return false;



			#elif defined(Sphere_Geometric)

			//Geometric Solution
			Vector3 hitNormal{};
			Vector3 originRayToSphere{ sphere.origin - ray.origin };
			float rDOTo{ Vector3::Dot(originRayToSphere,ray.direction) };

			float od_squared{ originRayToSphere.SqrMagnitude() - (rDOTo * rDOTo) };

			if (od_squared > (sphere.radius * sphere.radius))
				return false;
			float t{ rDOTo - sqrtf(Square(sphere.radius) - od_squared) };
				
			if (t > ray.min && t < ray.max)
			{
				if (!ignoreHitRecord)
				{
					hitRecord.didHit = true;
					hitRecord.t = t;
					hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
					hitRecord.materialIndex = sphere.materialIndex;
					hitRecord.normal = (hitRecord.origin - sphere.origin) / sphere.radius;
				}
				return true;
			}
			return false;


			#endif

		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			const float t{ Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3::Dot(ray.direction, plane.normal) };

			if (!(t >= ray.min && t <= ray.max))
				return false;
		
			hitRecord.t = t;
			hitRecord.didHit = true;
			hitRecord.materialIndex = plane.materialIndex;
			hitRecord.origin = ray.origin + (t * ray.direction);
			hitRecord.normal = plane.normal;
			return hitRecord.didHit;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS

		inline bool SlabTest(const TriangleMesh& mesh, const Ray& ray)
		{
			float tx1 = (mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x;
			float tx2 = (mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x;

			float tMin = std::min(tx1, tx2);
			float tMax = std::max(tx1, tx2);

			float ty1 = (mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y;
			float ty2 = (mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y;

			tMin = std::max(tMin, std::min(ty1, ty2));
			tMax = std::min(tMax, std::max(ty1, ty2));

			float tz1 = (mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z;
			float tz2 = (mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z;

			tMin = std::max(tMin, std::min(tz1, tz2));
			tMax = std::min(tMax, std::max(tz1, tz2));

			
			return tMax > 0 && tMax >= tMin;
		}

		inline bool HitTest_TriangleIntersection(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//Vector3 triangleNormal{ triangle.normal }; 
			const float dot(Vector3::Dot(triangle.normal, ray.direction));

			if (dot == 0)
				return false;
			if (dot > 0)
			{
				if (ignoreHitRecord && TriangleCullMode::FrontFaceCulling == triangle.cullMode)
					return false;
				if (!ignoreHitRecord && TriangleCullMode::BackFaceCulling == triangle.cullMode)
					return false;
			}
			if (dot < 0)
			{
				if (ignoreHitRecord && TriangleCullMode::BackFaceCulling == triangle.cullMode)
					return false;
				if (!ignoreHitRecord && TriangleCullMode::FrontFaceCulling == triangle.cullMode)
					return false;
			}

			const Vector3 center{ (triangle.v0 + triangle.v1 + triangle.v2) / 3 };
			Vector3 L{ center - ray.origin };
			float t{ (Vector3::Dot(L, triangle.normal)) / (Vector3::Dot(ray.direction, triangle.normal)) };

			float tMax{ ray.max };
			float tMin{ ray.min };

			if (t < tMin || t > tMax)
				return false;

			//Vector3 hitPoint{ ray.origin + t * ray.direction };
			//Vector3 edge0{ triangle.v1 - triangle.v0 };
			//Vector3 edge1{ triangle.v2 - triangle.v1 };
			//Vector3 edge2{ triangle.v0 - triangle.v2 };
			//Vector3 pointToSide0{ hitPoint - triangle.v0 };
			//Vector3 pointToSide1{ hitPoint - triangle.v1 };
			//Vector3 pointToSide2{ hitPoint - triangle.v2 };
			if (Vector3::Dot(triangle.normal, Vector3::Cross(triangle.v1 - triangle.v0, (ray.origin + t * ray.direction) - triangle.v0)) < 0)
				return false;
			if (Vector3::Dot(triangle.normal, Vector3::Cross(triangle.v2 - triangle.v1, (ray.origin + t * ray.direction) - triangle.v1)) < 0)
				return false;
			if (Vector3::Dot(triangle.normal, Vector3::Cross(triangle.v0 - triangle.v2, (ray.origin + t * ray.direction) - triangle.v2)) < 0)
				return false;


			hitRecord.materialIndex = triangle.materialIndex;
			hitRecord.origin = (ray.origin + t * ray.direction);
			hitRecord.normal = triangle.normal;
			hitRecord.t = t;
			return hitRecord.didHit = true;
		}

		inline bool HitTest_Triangle_MollerTrumbore(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{

			const Vector3 v0v1 = triangle.v1 - triangle.v0;
			const Vector3 v0v2 = triangle.v2 - triangle.v0;
			const Vector3 pvec = Vector3::Cross(ray.direction, v0v2);
			const float determinant = Vector3::Dot(v0v1, pvec);

			if (determinant == 0)
			{
			return false;
			}
			if (determinant < 0)
			{
				if (ignoreHitRecord && TriangleCullMode::FrontFaceCulling == triangle.cullMode)
					return false;
				if (!ignoreHitRecord && TriangleCullMode::BackFaceCulling == triangle.cullMode)
					return false;
			}
			else if (determinant > 0)
			{
				if (ignoreHitRecord && TriangleCullMode::BackFaceCulling == triangle.cullMode)
					return false;
				if (!ignoreHitRecord && TriangleCullMode::FrontFaceCulling == triangle.cullMode)
					return false;
			}

			const float invDet = 1 / determinant;

			const Vector3 tvec = ray.origin - triangle.v0;
			const float u = Vector3::Dot(tvec, pvec) * invDet;
			if (u < 0 || u > 1) 
				return false;

			const Vector3 qvec = Vector3::Cross(tvec, v0v1);
			const float v = Vector3::Dot(ray.direction, qvec) * invDet;
			if (v < 0 || u + v > 1) 
				return false;

			const float t = Vector3::Dot(v0v2, qvec) * invDet;
			
			if (t < ray.min || t > ray.max)
				return false;

			hitRecord.didHit = true;
			hitRecord.materialIndex = triangle.materialIndex;
			hitRecord.origin = ray.origin + (t * ray.direction);
			hitRecord.normal = triangle.normal;
			hitRecord.t = t;
			return true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			#if defined(Triangle_Geometric)
			return HitTest_TriangleIntersection(triangle, ray, hitRecord, ignoreHitRecord);
			#elif defined(Triangle_MollerTrumbore)
			return HitTest_Triangle_MollerTrumbore(triangle, ray, hitRecord, ignoreHitRecord);
			#endif
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			if (!SlabTest(mesh, ray))
				return false;

			int sizeIndices{ static_cast<int>(mesh.indices.size()) };
			Triangle tempTriangle;
			tempTriangle.cullMode = mesh.cullMode;
			tempTriangle.materialIndex = mesh.materialIndex;
			float distance{ FLT_MAX };


			HitRecord hitRecordTemTriangle{};
			for(int index{}; index < sizeIndices; ++index)
			{
				tempTriangle.v0 = mesh.transformedPositions[mesh.indices[index]];
				tempTriangle.v1 = mesh.transformedPositions[mesh.indices[++index]];
				tempTriangle.v2 = mesh.transformedPositions[mesh.indices[++index]];
				tempTriangle.normal = mesh.transformedNormals[index / 3];

				if (HitTest_Triangle(tempTriangle, ray, hitRecordTemTriangle, ignoreHitRecord))
				{
					if (ignoreHitRecord)
					{
						return true;
					}
					if (hitRecordTemTriangle.didHit && hitRecordTemTriangle.t < distance)
					{
						distance = hitRecordTemTriangle.t;
						hitRecord = hitRecordTemTriangle;
					}
				}
			}
			return hitRecord.didHit;

		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			if (light.type == LightType::Directional)
			{
				return -light.direction;
			}
			 
			Vector3 lightDirection{ light.origin - origin };

			return lightDirection;
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{

			if (light.type == LightType::Point)
			{
				const float intensity{ light.intensity };
				const Vector3 lightPosition{ light.origin };
				const Vector3 pointToShade{ target };
				const ColorRGB lightColor{ light.color };

				return lightColor * (intensity / (lightPosition - pointToShade).SqrMagnitude());

			}
			if (light.type == LightType::Directional)
			{
				const float intensity{ light.intensity };
				const ColorRGB lightColor{ light.color };
				return lightColor * intensity;
			}
			return ColorRGB{};
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}