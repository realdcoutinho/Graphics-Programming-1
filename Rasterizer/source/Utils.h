#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

//#define DISABLE_OBJ

namespace dae
{
	namespace Utils
	{

		//inline bool HitTest_TriangleIntersection(const Triangle& triangle, const Vector3& pixel)
		//{
		//	//Vector3 hitPoint{ ray.origin + t * ray.direction };
		//	//Vector3 edge0{ triangle.v1 - triangle.v0 };
		//	//Vector3 edge1{ triangle.v2 - triangle.v1 };
		//	//Vector3 edge2{ triangle.v0 - triangle.v2 };
		//	//Vector3 pointToSide0{ hitPoint - triangle.v0 };
		//	//Vector3 pointToSide1{ hitPoint - triangle.v1 };
		//	//Vector3 pointToSide2{ hitPoint - triangle.v2 };
		//	if (Vector3::Dot(triangle.normal, Vector3::Cross(triangle.v1 - triangle.v0, pixel - triangle.v0)) < 0)
		//		return false;
		//	if (Vector3::Dot(triangle.normal, Vector3::Cross(triangle.v2 - triangle.v1, pixel - triangle.v1)) < 0)
		//		return false;
		//	if (Vector3::Dot(triangle.normal, Vector3::Cross(triangle.v0 - triangle.v2, pixel - triangle.v2)) < 0)
		//		return false;
		//	else
		//		return false;
		//}

		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding = true)
		{
#ifdef DISABLE_OBJ

			//TODO: Enable the code below after uncommenting all the vertex attributes of DataTypes::Vertex
			// >> Comment/Remove '#define DISABLE_OBJ'
			assert(false && "OBJ PARSER not enabled! Check the comments in Utils::ParseOBJ");
			
#else

			std::ifstream file(filename);
			if (!file)
				return false;

			std::vector<Vector3> positions{};
			std::vector<Vector3> normals{};
			std::vector<Vector2> UVs{};

			vertices.clear();
			indices.clear();

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

					positions.emplace_back(x, y, z);
				}
				else if (sCommand == "vt")
				{
					// Vertex TexCoord
					float u, v;
					file >> u >> v;
					UVs.emplace_back(u, 1 - v);
				}
				else if (sCommand == "vn")
				{
					// Vertex Normal
					float x, y, z;
					file >> x >> y >> z;

					normals.emplace_back(x, y, z);
				}
				else if (sCommand == "f")
				{
					//if a face is read:
					//construct the 3 vertices, add them to the vertex array
					//add three indices to the index array
					//add the material index as attibute to the attribute array
					//
					// Faces or triangles
					Vertex vertex{};
					size_t iPosition, iTexCoord, iNormal;

					uint32_t tempIndices[3];
					for (size_t iFace = 0; iFace < 3; iFace++)
					{
						// OBJ format uses 1-based arrays
						file >> iPosition;
						vertex.position = positions[iPosition - 1];

						if ('/' == file.peek())//is next in buffer ==  '/' ?
						{
							file.ignore();//read and ignore one element ('/')

							if ('/' != file.peek())
							{
								// Optional texture coordinate
								file >> iTexCoord;
								vertex.uv = UVs[iTexCoord - 1];
							}

							if ('/' == file.peek())
							{
								file.ignore();

								// Optional vertex normal
								file >> iNormal;
								vertex.normal = normals[iNormal - 1];
							}
						}

						vertices.push_back(vertex);
						tempIndices[iFace] = uint32_t(vertices.size()) - 1;
						//indices.push_back(uint32_t(vertices.size()) - 1);
					}

					indices.push_back(tempIndices[0]);
					if (flipAxisAndWinding) 
					{
						indices.push_back(tempIndices[2]);
						indices.push_back(tempIndices[1]);
					}
					else
					{
						indices.push_back(tempIndices[1]);
						indices.push_back(tempIndices[2]);
					}
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');
			}

			//Cheap Tangent Calculations
			for (uint32_t i = 0; i < indices.size(); i += 3)
			{
				uint32_t index0 = indices[i];
				uint32_t index1 = indices[size_t(i) + 1];
				uint32_t index2 = indices[size_t(i) + 2];

				const Vector3& p0 = vertices[index0].position;
				const Vector3& p1 = vertices[index1].position;
				const Vector3& p2 = vertices[index2].position;
				const Vector2& uv0 = vertices[index0].uv;
				const Vector2& uv1 = vertices[index1].uv;
				const Vector2& uv2 = vertices[index2].uv;

				const Vector3 edge0 = p1 - p0;
				const Vector3 edge1 = p2 - p0;
				const Vector2 diffX = Vector2(uv1.x - uv0.x, uv2.x - uv0.x);
				const Vector2 diffY = Vector2(uv1.y - uv0.y, uv2.y - uv0.y);
				float r = 1.f / Vector2::Cross(diffX, diffY);

				Vector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
				vertices[index0].tangent += tangent;
				vertices[index1].tangent += tangent;
				vertices[index2].tangent += tangent;
			}

			//Fix the tangents per vertex now because we accumulated
			for (auto& v : vertices)
			{
				v.tangent = Vector3::Reject(v.tangent, v.normal).Normalized();

				if(flipAxisAndWinding)
				{
					v.position.z *= -1.f;
					v.normal.z *= -1.f;
					v.tangent.z *= -1.f;
				}

			}

			return true;
#endif
		}



#pragma warning(pop)
	

#pragma region helperFunctions

		bool FrustumCulling(const Vertex_Out& vertex)
		{

			// Switched trues and falses in here for own better understanding
			if (vertex.position.x > 1.0f || vertex.position.x < -1.0f)
				return true;
			if (vertex.position.y > 1.0f || vertex.position.y < -1.0f)
				return true;
			//if (vertex.position.z > 1.0f || vertex.position.z < 0.0f)
			//	return false;

			return false;




			//if (vertex.position.x > 1.0f || vertex.position.x < -1.0f)
			//	return false;
			//if (vertex.position.y > 1.0f || vertex.position.y < -1.0f)
			//	return false;
			//if (vertex.position.z > 1.0f || vertex.position.z < 0.0f)
			//	return false;

			//return true;
		}

		void TransformToRasterSpace(Vertex_Out& vertex, const int width, const int height)
		{
			vertex.position.x = (vertex.position.x + 1) / 2 * static_cast<float>(width);
			vertex.position.y = (1 - vertex.position.y) / 2 * static_cast<float>(height);
		}

		void TransformToRasterSpace(Vertex_Out& v0, Vertex_Out& v1, Vertex_Out& v2, const int width, const int height)
		{
			v0.position.x = (v0.position.x + 1) / 2 * static_cast<float>(width);
			v0.position.y = (1 - v0.position.y) / 2 * static_cast<float>(height);

			v1.position.x = (v1.position.x + 1) / 2 * static_cast<float>(width);
			v1.position.y = (1 - v1.position.y) / 2 * static_cast<float>(height);

			v2.position.x = (v2.position.x + 1) / 2 * static_cast<float>(width);
			v2.position.y = (1 - v2.position.y) / 2 * static_cast<float>(height);
		}

		void CalculateAABB(const std::vector<Vertex_Out>& vertices, Vector3& minAABB, Vector3& maxAABB, const int maxWidth, const int maxHeight)
		{
			float offset{ 1.0f }; //removes the black lines between each triangle // however not between triangles vertically, only those horizontally next to eahc other

			minAABB = vertices[0].position;
			maxAABB = vertices[0].position;

			for (const Vertex_Out& pos : vertices)
			{
				//get min
				if (pos.position.x < minAABB.x)
					minAABB.x = pos.position.x - offset;
				if (pos.position.y < minAABB.y)
					minAABB.y = pos.position.y - offset;
				//if (pos.position.z < minAABB.z)
				//	minAABB.z = pos.position.z - offset;

				//get max
				if (pos.position.x > maxAABB.x)
					maxAABB.x = pos.position.x + offset;
				if (pos.position.y > maxAABB.y)
					maxAABB.y = pos.position.y + offset;
				//if (pos.position.z > maxAABB.z)
				//	maxAABB.z = pos.position.z + offset;
			}

			minAABB.x = Clamp(minAABB.x, 0.0f, static_cast<float>(maxWidth));
			minAABB.y = Clamp(minAABB.y, 0.0f, static_cast<float>(maxHeight));
			maxAABB.x = Clamp(maxAABB.x, 0.0f, static_cast<float>(maxWidth));
			maxAABB.y = Clamp(maxAABB.y, 0.0f, static_cast<float>(maxHeight));
		}

		float CalculateArea(const Vector2& v0, const Vector2& v1, const Vector2& v2)
		{
			return Vector2::Cross((v1 - v0), v2 - v0);
		}

		float CalculateWeights(const Vector2& vertex1, const Vector2& vertex2, const Vector2& pixel, const float area)
		{
			return Vector2::Cross(vertex2 - vertex1, pixel - vertex1) / area;
		}

		float CalculateInterpolatedZ(const std::vector<Vertex_Out>& triangle, const float w0, const float w1, const float w2)
		{
			float part1 = (1.0f / triangle[0].position.z) * w0;
			float part2 = (1.0f / triangle[1].position.z) * w1;
			float part3 = (1.0f / triangle[2].position.z) * w2;

			float total = part1 + part2 + part3;

			return (1 / total); // z interpolated;
		}

		float CalculateInterpolatedW(const std::vector<Vertex_Out>& triangle, const float w0, const float w1, const float w2)
		{
			float part1 = (1.0f / triangle[0].position.w) * w0;
			float part2 = (1.0f / triangle[1].position.w) * w1;
			float part3 = (1.0f / triangle[2].position.w) * w2;

			float total = part1 + part2 + part3;

			return (1 / total); // w interpolated;
		}

		Vector2 CalculateInterpolatedUV(const std::vector<Vertex_Out>& triangle, const float w0, const float w1, const float w2, const float interpolatedW) 
		{
			return ((triangle[0].uv / triangle[0].position.w) * w0 + (triangle[1].uv / triangle[1].position.w) * w1 + (triangle[2].uv / triangle[2].position.w) * w2) * interpolatedW;
		}

		Vector3 CalculateInterpolatedNormal(const std::vector<Vertex_Out>& triangle, const float w0, const float w1, const float w2, const float interpolatedW)
		{
			return ((triangle[0].normal / triangle[0].position.w) * w0 + (triangle[1].normal / triangle[1].position.w) * w1 + (triangle[2].normal / triangle[2].position.w) * w2) * interpolatedW;
		}

		Vector3 CalculateInterpolatedTangent(const std::vector<Vertex_Out>& triangle, const float w0, const float w1, const float w2, const float interpolatedW)
		{
			return ((triangle[0].tangent / triangle[0].position.w) * w0 + (triangle[1].tangent / triangle[1].position.w) * w1 + (triangle[2].tangent / triangle[2].position.w) * w2) * interpolatedW;
		}

		Vector4 CalculateInterpolatedPixelPosition(const std::vector<Vertex_Out>& triangle, const float w0, const float w1, const float w2, const float interpolatedW)
		{
			return (triangle[0].position * w0 + triangle[1].position * w1 + triangle[2].position * w2) * interpolatedW;
		}


		Vector3 CalculateInterpolatedViewDirection(const std::vector<Vertex_Out>& triangle, const float w0, const float w1, const float w2, const float interpolatedW)
		{
			return ((triangle[0].viewDirection / triangle[0].position.w) * w0 + (triangle[1].viewDirection / triangle[1].position.w) * w1 + (triangle[2].viewDirection / triangle[2].position.w) * w2) * interpolatedW;
		}

		Vertex_Out CalculateInterpolatedVerTex_Out(const std::vector<Vertex_Out>& triangle, const float w0, const float w1, const float w2, const float interpolatedW)
		{
			//return ((triangle[0].viewDirection / triangle[0].position.w) * w0 + (triangle[1].viewDirection / triangle[1].position.w) * w1 + (triangle[2].viewDirection / triangle[2].position.w) * w2) * interpolatedW;
			const Vector4 interpolatedPixelPosition{ CalculateInterpolatedPixelPosition(triangle, w0, w1, w2, interpolatedW) };
			const Vector2 interpolatedUV{ Utils::CalculateInterpolatedUV(triangle, w0, w1, w2, interpolatedW) };
			const Vector3 interpolatedNormal{ Utils::CalculateInterpolatedNormal(triangle, w0, w1,w2, interpolatedW) };
			const Vector3 interpolatedTangent{ Utils::CalculateInterpolatedTangent(triangle, w0, w1,w2, interpolatedW) };
			const Vector3 interpolatedViewDirection{ Utils::CalculateInterpolatedViewDirection(triangle, w0, w1,w2, interpolatedW) };
			return Vertex_Out { interpolatedPixelPosition, {0, 0, 0}, interpolatedUV, interpolatedNormal, interpolatedTangent, interpolatedViewDirection };
		}


		float Remap(float value)
		{
			float min = 0.99f; //value changed on purpose to fit the vehicle better // it used to be 0.985f
			float max = 1.0f;

			float newValue = value - min;

			return newValue / (max - min);

		}

#pragma region noUse
		//Not in Use no longer
		bool SlabTest(const Vector3& minAABB, const Vector3& maxAABB, const Vector2& pixel)  //NOT A BOUNDING BOX PER SAY. ASK IN CLASS
		{
			//check X
			if (pixel.x < minAABB.x || pixel.x > maxAABB.x)
				return false;
			if (pixel.y < minAABB.y || pixel.y > maxAABB.y)
				return false;

			return true;
		}

		float sign(const Vector3& vertex1, const Vector3& vertex2, const Vector3& pixel)
		{
			return (pixel.x - vertex2.x) * (vertex1.y - vertex2.y) - (vertex1.x - vertex2.x) * (pixel.y - vertex2.y);
		}

		bool PointInTriangle(const std::vector<Vertex_Out>& vertex, const Vector3& pixel) 
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

#pragma endregion noUse



#pragma endregion helperFunctions


#pragma region shadingFunction

		ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			return { kd * cd / PI };
		}

		ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			return { kd * cd / PI };
		}

		ColorRGB Phong(float ks, float exp, const Vector3& lightVector, const Vector3& viewDirection, const Vector3& normal)
		{
			const Vector3 reflect{ lightVector - 2 * (Vector3::Dot(normal, lightVector)) * normal };
			const float cosineAlpha{ std::max(0.0f, Vector3::Dot(reflect, viewDirection)) };
			const float phongSpecularReflection{ ks * (powf(cosineAlpha, exp)) };
			const ColorRGB Phong{ phongSpecularReflection , phongSpecularReflection , phongSpecularReflection };
			return Phong;
		}

		ColorRGB GetRadiance(const Light& light, const Vector3& target = { 0, 0, 0 })
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
				const float intensity{ light.intensity};
				const ColorRGB lightColor{ light .color};
				return lightColor * intensity;
			}
			//return ColorRGB{};
		}

#pragma endregion shadingFunctions


#pragma region LightUtils
		Light AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
		{
			Light l;
			l.origin = origin;
			l.intensity = intensity;
			l.color = color;
			l.type = LightType::Point;

			//m_Lights.emplace_back(l);
			//return &m_Lights.back();
			return l;
		}

		Light AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color, const float shininess)
		{
			Light l;
			l.direction = direction;
			l.intensity = intensity;
			l.color = color;
			l.shininess = shininess;
			l.type = LightType::Directional;

			//m_Lights.emplace_back(l);
			//return &m_Lights.back();
			return l;
		}
#pragma endregion LightUtils

}
}