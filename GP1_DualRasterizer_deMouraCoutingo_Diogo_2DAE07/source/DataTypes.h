#pragma once
#include "Math.h"
#include "vector"

#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	//struct Vertex_Input
	//{
	//	Vector3 position;
	//	ColorRGB color;
	//};
	//
	//struct Vertex_out
	//{
	//	Vector4 position;
	//	ColorRGB color;
	//};

	struct Vertex_PosCol
	{
		Vector3 position;
		ColorRGB color;
		Vector2 uv;
		Vector3 normal;
		Vector3 tangent;
	};

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
		float shininess{};
		ColorRGB ambient{};


		LightType type{};
	};
#pragma endregion


	struct Vertex
	{
		Vector3 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{}; //W3
		Vector3 normal{}; //W4
		Vector3 tangent{}; //W4
		Vector3 viewDirection{}; //W4
	};

	//struct Vertex_PosCol
	//{
	//	Vector3 position;
	//	ColorRGB color;
	//	Vector2 uv;
	//	Vector3 normal;
	//	Vector3 tangent;
	//};


	struct Vertex_Out
	{
		Vector4 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	struct MeshStruct
	{
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };

		std::vector<Vertex_Out> vertices_out{};
		Matrix worldMatrix{};
	};


			//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
//			static bool ParseOBJ(const std::string& filename, std::vector<Vertex_PosCol>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding = true)
//			{
//#ifdef DISABLE_OBJ
//
//				//TODO: Enable the code below after uncommenting all the vertex attributes of DataTypes::Vertex
//				// >> Comment/Remove '#define DISABLE_OBJ'
//				assert(false && "OBJ PARSER not enabled! Check the comments in Utils::ParseOBJ");
//
//#else
//
//				std::ifstream file(filename);
//				if (!file)
//					return false;
//
//				std::vector<Vector3> positions{};
//				std::vector<Vector3> normals{};
//				std::vector<Vector2> UVs{};
//
//				vertices.clear();
//				indices.clear();
//
//				std::string sCommand;
//				// start a while iteration ending when the end of file is reached (ios::eof)
//				while (!file.eof())
//				{
//					//read the first word of the string, use the >> operator (istream::operator>>) 
//					file >> sCommand;
//					//use conditional statements to process the different commands	
//					if (sCommand == "#")
//					{
//						// Ignore Comment
//					}
//					else if (sCommand == "v")
//					{
//						//Vertex
//						float x, y, z;
//						file >> x >> y >> z;
//
//						positions.emplace_back(x, y, z);
//					}
//					else if (sCommand == "vt")
//					{
//						// Vertex TexCoord
//						float u, v;
//						file >> u >> v;
//						UVs.emplace_back(u, 1 - v);
//					}
//					else if (sCommand == "vn")
//					{
//						// Vertex Normal
//						float x, y, z;
//						file >> x >> y >> z;
//
//						normals.emplace_back(x, y, z);
//					}
//					else if (sCommand == "f")
//					{
//						//if a face is read:
//						//construct the 3 vertices, add them to the vertex array
//						//add three indices to the index array
//						//add the material index as attibute to the attribute array
//						//
//						// Faces or triangles
//						Vertex_PosCol vertex{};
//						size_t iPosition, iTexCoord, iNormal;
//
//						uint32_t tempIndices[3];
//						for (size_t iFace = 0; iFace < 3; iFace++)
//						{
//							// OBJ format uses 1-based arrays
//							file >> iPosition;
//							vertex.position = positions[iPosition - 1];
//
//							if ('/' == file.peek())//is next in buffer ==  '/' ?
//							{
//								file.ignore();//read and ignore one element ('/')
//
//								if ('/' != file.peek())
//								{
//									// Optional texture coordinate
//									file >> iTexCoord;
//									vertex.uv = UVs[iTexCoord - 1];
//								}
//
//								if ('/' == file.peek())
//								{
//									file.ignore();
//
//									// Optional vertex normal
//									file >> iNormal;
//									vertex.normal = normals[iNormal - 1]; //fix this line
//								}
//							}
//
//							vertices.push_back(vertex);
//							tempIndices[iFace] = uint32_t(vertices.size()) - 1;
//							indices.push_back(uint32_t(vertices.size()) - 1);
//						}
//
//						indices.push_back(tempIndices[0]);
//						if (flipAxisAndWinding)
//						{
//							indices.push_back(tempIndices[2]);
//							indices.push_back(tempIndices[1]);
//						}
//						else
//						{
//							indices.push_back(tempIndices[1]);
//							indices.push_back(tempIndices[2]);
//						}
//					}
//					//read till end of line and ignore all remaining chars
//					file.ignore(1000, '\n');
//				}
//
//				//Cheap Tangent Calculations
//				for (uint32_t i = 0; i < indices.size(); i += 3)
//				{
//					uint32_t index0 = indices[i];
//					uint32_t index1 = indices[size_t(i) + 1];
//					uint32_t index2 = indices[size_t(i) + 2];
//
//					const Vector3& p0 = vertices[index0].position;
//					const Vector3& p1 = vertices[index1].position;
//					const Vector3& p2 = vertices[index2].position;
//					const Vector2& uv0 = vertices[index0].uv;
//					const Vector2& uv1 = vertices[index1].uv;
//					const Vector2& uv2 = vertices[index2].uv;
//
//					const Vector3 edge0 = p1 - p0;
//					const Vector3 edge1 = p2 - p0;
//					const Vector2 diffX = Vector2(uv1.x - uv0.x, uv2.x - uv0.x);
//					const Vector2 diffY = Vector2(uv1.y - uv0.y, uv2.y - uv0.y);
//					float r = 1.f / Vector2::Cross(diffX, diffY);
//
//					Vector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
//					vertices[index0].tangent += tangent;
//					vertices[index1].tangent += tangent;
//					vertices[index2].tangent += tangent;
//				}
//
//				//Fix the tangents per vertex now because we accumulated
//				for (auto& v : vertices)
//				{
//					v.tangent = Vector3::Reject(v.tangent, v.normal).Normalized();
//
//					if (flipAxisAndWinding)
//					{
//						v.position.z *= -1.f;
//						v.normal.z *= -1.f;
//						v.tangent.z *= -1.f;
//					}
//				}
//
//				return true;
//#endif
//			}



#pragma warning(pop)
		}

//	struct Vertex
//	{
//		Vector3 position{};
//		ColorRGB color{colors::White};
//		Vector2 uv{}; //W3
//		Vector3 normal{}; //W4
//		Vector3 tangent{}; //W4
//		Vector3 viewDirection{}; //W4
//	};
//
//	struct Vertex_Out
//	{
//		Vector4 position{};
//		ColorRGB color{ colors::White };
//		Vector2 uv{};
//		Vector3 normal{};
//		Vector3 tangent{};
//		Vector3 viewDirection{};
//	};
//
//	enum class PrimitiveTopology
//	{
//		TriangleList,
//		TriangleStrip
//	};
//
//	struct Mesh
//	{
//		std::vector<Vertex> vertices{};
//		std::vector<uint32_t> indices{};
//		PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };
//
//		std::vector<Vertex_Out> vertices_out{};
//		Matrix worldMatrix{};
//	};
//
//
//
//
//	struct Triangle
//	{
//		Triangle() = default;
//		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Vector3& _normal) :
//			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }, normal{ _normal.Normalized() }
//		{
//		}
//
//		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2) :
//			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }
//		{
//			positions.push_back(v0);
//			positions.push_back(v1);
//			positions.push_back(v2);
//			//const Vector3 edgeV0V1 = v1 - v0;
//			//const Vector3 edgeV0V2 = v2 - v0;
//			//normal = Vector3::Cross(edgeV0V1, edgeV0V2).Normalized();
//		}
//
//		Vector3 v0{};
//		Vector3 v1{};
//		Vector3 v2{};
//
//		Vector3 normal{};
//
//		//TriangleCullMode cullMode{};
//		//unsigned char materialIndex{};
//
//
//		std::vector<Vector3> positionsProjectionStage{};
//		std::vector<Vector3> positionsCameraSpace{};
//		std::vector<Vector3> positionsNDC{};
//		std::vector<Vector3> positions{};
//
//		void TransformToScreenSpace(float width, float height)
//		{
//			for (auto& pos : positions)
//			{
//				pos.x = (pos.x + 1) / 2 * width;
//				pos.y = (1 - pos.y) / 2 * height;
//			}
//
//			v0 = positions[0];
//			v1 = positions[1];
//			v2 = positions[2];
//		}
//
//		void TransformToProjectedStage()
//		{
//
//			for (auto& pos : positions)
//			{
//				float projectedX = pos.x / pos.z;
//				float projectedY = pos.y / pos.z;
//				Vector3 projected{ projectedX, projectedY, pos.z };
//				pos = projected;
//				positionsProjectionStage.emplace_back(pos);
//			}
//			v0 = positionsProjectionStage[0];
//			v1 = positionsProjectionStage[1];
//			v2 = positionsProjectionStage[2];
//
//		}
//	};
//
//#pragma region LIGHT
//	enum class LightType
//	{
//		Point,
//		Directional
//	};
//
//	struct Light
//	{
//		Vector3 origin{};
//		Vector3 direction{};
//		ColorRGB color{};
//		float intensity{};
//		float shininess{};
//		ColorRGB ambient{};
//
//
//		LightType type{};
//	};
//#pragma endregion
