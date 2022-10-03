#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1

			Vector3 center{ sphere.origin };
			float cX{ center.x };
			float cY{ center.y };
			float cZ{ center.z };

			//Quadratic equation A B and C
			float A{ Vector3::Dot(ray.direction, ray.direction) };
			float B{ Vector3::Dot(2*ray.direction, ray.origin - sphere.origin)};
			float C{ Vector3::Dot(ray.origin - sphere.origin,  ray.origin - sphere.origin) - powf(sphere.radius, 2)};

			float discriminant{ powf(B, 2) - 4 * A * C};

			float tMax{ray.max};
			float tMin{ray.min};


			if (discriminant < 0)
			{
				//ray does not intersect
				return false;
			}
			if(discriminant == 0)
			{
				//ray touches the sphere in one point
				return false;
			}
			if (discriminant > 0)
			{
				//ray touches intersects the sphere in two points
				float t{ (-B - sqrt(discriminant)) / 2 * A };
				Vector3 p{ ray.origin + t*ray.direction};
				
				Vector3 vectorSphereCenterToP{ p.x - sphere.origin.x, p.y - sphere.origin.y, p.z - sphere.origin.z };
				//Vector3 vectorSphereCenterToP{ p.x - sphere.origin.x, p.y - sphere.origin.y, p.z - sphere.origin.z };
				
				hitRecord.didHit = true;
				hitRecord.materialIndex = sphere.materialIndex;
				hitRecord.normal = vectorSphereCenterToP.Normalized();
				hitRecord.origin = p;
				hitRecord.t = t;
				return hitRecord.didHit;
			}
			return false;
			
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

			float t{ Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3::Dot(ray.direction, plane.normal) };
			Vector3 p{ ray.origin + (t * ray.direction) };
			float pX{ ray.origin.x + (t * ray.direction.x) };
			float check{ Vector3::Dot(p - plane.origin, plane.normal) };

			float tMax{ ray.max };
			float tMin{ ray.min };

			

			//if (check >= -0.1 && check <= 0.1)
			//{
				if (t >= tMin && t <= tMax)
				{
					hitRecord.didHit = true;
					hitRecord.materialIndex = plane.materialIndex;
					hitRecord.normal = plane.normal;
					hitRecord.origin = ray.origin;
					hitRecord.t = t;
					return hitRecord.didHit;
				}
				return false;
			//}
			//return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
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
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
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
			////float x{ light.direction.x - origin.x };
			////float y{ light.direction.y - origin.y };
			////float z{ light.direction.z - origin.z };
			//// 
			////Vector3 directionVector{ x, y, z };
			////return directionVector;
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			assert(false && "No Implemented Yet!");
			return {};
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