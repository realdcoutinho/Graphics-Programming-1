#pragma once
#include "Math.h"
#include "vector"

namespace dae
{
	struct Vertex_Input
	{
		Vector3 position;
		ColorRGB color;
	};

	struct Vertex_out
	{
		Vector4 position;
		ColorRGB color;
	};

	struct Vertex_PosCol
	{
		Vector3 position;
		ColorRGB color;
	};



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
}